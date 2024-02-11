#ifndef CMW_TRANSPORT_DISPATCHER_SHM_DISPATCHER_H_
#define CMW_TRANSPORT_DISPATCHER_SHM_DISPATCHER_H_

#include <cmw/transport/dispatcher/dispatcher.h>
#include <cmw/transport/shm/segment.h>
#include <cmw/base/atomic_rw_lock.h>
#include <cmw/base/rw_lock_guard.h>
#include <cmw/transport/shm/notifier_base.h>
#include <cmw/base/macros.h>
#include <cmw/common/log.h>
#include <cmw/serialize/serializable.h>
namespace hnu    {
namespace cmw   {
namespace transport {

class ShmDispatcher;
using ShmDispatcherPtr = ShmDispatcher*;

class ShmDispatcher : public Dispatcher{

public:
    using SegmentContainer = std::unordered_map<uint64_t, SegmentPtr>;

    virtual ~ShmDispatcher();

    void Shutdown() override;

    template <typename MessageT>
    void AddListener(const RoleAttributes& self_attr,
                     const MessageListener<MessageT>& listener);
    
    template <typename MessageT>
    void AddListener(const RoleAttributes& self_attr,
                     const RoleAttributes& opposite_attr,
                     const MessageListener<MessageT>& listener);
    

private:
    void AddSegment(const RoleAttributes& self_attr);
    void ReadMessage(uint64_t channel_id, uint32_t block_index);
    void OnMessage(uint64_t channel_id, const std::shared_ptr<ReadableBlock>& rb,
                    const MessageInfo& msg_info);
    void ThreadFunc();
    bool Init();

    uint64_t host_id_;
    SegmentContainer segments_;
    std::unordered_map<uint64_t, uint32_t> previous_indexs_;
    AtomicRWLock segments_lock_;
    std::thread thread_;
    NotifierPtr notifier_;

    //全局单例
    DECLARE_SINGLETON(ShmDispatcher)
};



template <typename MessageT>
void ShmDispatcher::AddListener(const RoleAttributes& self_attr,
                                const MessageListener<MessageT>& listener){

        auto listener_adapter = [listener](const std::shared_ptr<ReadableBlock>& rb,
                                          const MessageInfo& msg_info){
           auto msg = std::make_shared<MessageT>();
            //数据反序列化
           serialize::DataStream ds(reinterpret_cast<char*>(rb->buf) , rb->block->msg_size());
           ds >> *msg;
           //感觉这里msg_info是否也需要反序列化
           //执行回调
           listener(msg, msg_info);
        };

        Dispatcher::AddListener<ReadableBlock>(self_attr, listener_adapter);
        AddSegment(self_attr);
}

template <typename MessageT>
void ShmDispatcher::AddListener(const RoleAttributes& self_attr,
                                const RoleAttributes& opposite_attr,
                                const MessageListener<MessageT>& listener) {
  // FIXME: make it more clean
  auto listener_adapter = [listener](const std::shared_ptr<ReadableBlock>& rb,
                                     const MessageInfo& msg_info) {
    auto msg = std::make_shared<MessageT>();
            //数据反序列化
    serialize::DataStream ds(reinterpret_cast<char*>(rb->buf) , rb->block->msg_size());
    ds >> *msg;
    listener(msg, msg_info);
  };

  Dispatcher::AddListener<ReadableBlock>(self_attr, opposite_attr,
                                         listener_adapter);
  AddSegment(self_attr);
}




}
}
}

#endif