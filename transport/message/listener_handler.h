#ifndef CMW_TRANSPORT_MESSAGE_LISTENER_HANDLER_H_
#define CMW_TRANSPORT_MESSAGE_LISTENER_HANDLER_H_


#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <cmw/transport/message/message_info.h>
#include <cmw/base/signal.h>
#include <cmw/base/atomic_rw_lock.h>
#include <cmw/serialize/data_stream.h>
#include <cmw/common/log.h>
namespace hnu    {
namespace cmw   {
namespace transport {


using hnu::cmw::base::AtomicRWLock;
using hnu::cmw::base::ReadLockGuard;
using hnu::cmw::base::WriteLockGuard;

class ListenerHandlerBase;
using ListenerHandlerBasePtr = std::shared_ptr<ListenerHandlerBase>;

class ListenerHandlerBase
{

public:
    ListenerHandlerBase() {}
    virtual ~ListenerHandlerBase() {}

    virtual void Disconnect(uint64_t self_id) = 0;
    virtual void Disconnect(uint64_t slef_id, uint64_t oppo_id) = 0;
   // inline bool IsRawMessage() const { return is_raw_message_; }
    virtual void RunFromString(const std::string& str,
                                const MessageInfo& msg_info) = 0;
protected:
    bool is_raw_message_ = false;
};


template <typename MessageT>
class ListenerHandler : public ListenerHandlerBase{

public:
    using Message = std::shared_ptr<MessageT>;  //一个MessageT的指针

    using MessageSignal = base::Signal<const Message&, const MessageInfo&>; //监听MessageT的信号

    using Listener = std::function<void(const Message&, const MessageInfo&)>; //回调函数Listener的定义
    using MessageConnection = 
            base::Connection<const Message&, const MessageInfo&>;  //MessageT连接关系
    using ConnectionMap = std::unordered_map<uint64_t, MessageConnection>;

    ListenerHandler() {}
    virtual ~ListenerHandler() {}

    void Connect(uint64_t self_id, const Listener& listener);
    void Connect(uint64_t self_id, uint64_t oppo_id, const Listener& listener);

    void Disconnect(uint64_t self_id) override;
    void Disconnect(uint64_t self_id, uint64_t oppo_id) override;

    void Run(const Message& msg, const MessageInfo& msg_info);
    void RunFromString(const std::string& str,
                     const MessageInfo& msg_info) override;

private:
    using SignalPtr = std::shared_ptr<MessageSignal>;
    using MessageSignalMap = std::unordered_map<uint64_t, SignalPtr>;


    MessageSignal signal_;     //

    ConnectionMap signal_conns_; // key: self_id , 保存着信号的连接关系


    MessageSignalMap signals_;  //key: oppo_id

    std::unordered_map<uint64_t, ConnectionMap> signals_conns_;


    base::AtomicRWLock rw_lock_;
/**
 * @brief  self_id 是读者的Endpoint的id，即自己的id
 * @brief  oppo_id 是写者的Endpoint的id。即对方的id
 */
};

template <typename MessageT>
void ListenerHandler<MessageT>::Connect(uint64_t self_id, const Listener& listener){
    auto connection = signal_.Connect(listener);//为signal_连接一个槽函数
    if(!connection.IsConnected())
    {
        return;
    }

    //加锁
    WriteLockGuard<AtomicRWLock> lock(rw_lock_);
    //每个实体的id是唯一的,同一进程下的多个receiver有可能监听同一channel，但是他们的id是唯一的
    signal_conns_[self_id] = connection;
}

template <typename MessageT>
void ListenerHandler<MessageT>::Connect(uint64_t self_id, uint64_t oppo_id, const Listener& listener){

    WriteLockGuard<AtomicRWLock> lock(rw_lock_);
    if(signals_.find(oppo_id) == signals_.end())
    {
        //新建一个MessageSignal
        signals_[oppo_id] = std::make_shared<MessageSignal>();
    }
    //为id名为oppo_id的信号添加槽listener
    auto connection =signals_[oppo_id]->Connect(listener);
    if(!connection.IsConnected()){
        std::cout << oppo_id << " " << self_id << " connect failed!" << std::endl;
    }

    // 一个writer可能对应着很多个reader，所以对于同一个writer的多个reader用一张map保存起来
    if(signals_conns_.find(oppo_id) == signals_conns_.end())
    {
        signals_conns_[oppo_id]  = ConnectionMap();
    }

    signals_conns_[oppo_id][self_id] = connection;

}

template <typename MessageT>
void ListenerHandler<MessageT>::Disconnect(uint64_t self_id){
WriteLockGuard<AtomicRWLock> lock(rw_lock_);
  if (signal_conns_.find(self_id) == signal_conns_.end()) {
    return;
  }

  signal_conns_[self_id].Disconnect();
  signal_conns_.erase(self_id);
}

template <typename MessageT>
void ListenerHandler<MessageT>::Disconnect(uint64_t self_id, uint64_t oppo_id){
 
  WriteLockGuard<AtomicRWLock> lock(rw_lock_);
  if (signals_conns_.find(oppo_id) == signals_conns_.end()) {
    return;
  }

  if (signals_conns_[oppo_id].find(self_id) == signals_conns_[oppo_id].end()) {
    return;
  }

  signals_conns_[oppo_id][self_id].Disconnect();
  signals_conns_[oppo_id].erase(self_id);


}


template <typename MessageT>
void ListenerHandler<MessageT>::Run(const Message& msg,
                                    const MessageInfo& msg_info) {
    //执行回调                                 
    signal_(msg, msg_info);
    //
    uint64_t oppo_id = msg_info.sender_id().HashValue();    
    ReadLockGuard<AtomicRWLock> lock(rw_lock_);   
    if (signals_.find(oppo_id) == signals_.end()) {
        return;
    }  

    (*signals_[oppo_id])(msg, msg_info);        

}

template <typename MessageT>
void ListenerHandler<MessageT>::RunFromString(const std::string& str,
                                              const MessageInfo& msg_info) {
  // auto msg = std::make_shared<MessageT>();
  // serialize::DataStream ds(str);
  // ds >> *msg;

  // Run(msg,msg_info);

  AERROR << "RunFromString Error";

}





}
}
}




#endif