
#ifndef CMW_TRANSPORT_DISPATCHER_DISPATCHER_H_
#define CMW_TRANSPORT_DISPATCHER_DISPATCHER_H_

#include <atomic>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <cmw/config/RoleAttributes.h>
#include <cmw/base/atmoic_hash_map.h>
#include <cmw/base/atomic_rw_lock.h>
#include <cmw/transport/message/message_info.h>
#include <cmw/transport/message/listener_handler.h>
#include <cmw/common/global_data.h>
#include <cmw/common/log.h>

namespace hnu    {
namespace cmw   {
namespace transport {

using namespace common;
using hnu::cmw::base::AtomicHashMap;
using hnu::cmw::base::AtomicRWLock;

using namespace config;
class Dispatcher;
using DispatcherPtr = std::shared_ptr<Dispatcher>;

template <typename MessageT>
using MessageListener =
    std::function<void(const std::shared_ptr<MessageT>&, const MessageInfo&)>;

class Dispatcher {

public:
    Dispatcher();
    virtual ~Dispatcher();

    virtual void Shutdown();

    template <typename MessageT>
    void AddListener(const RoleAttributes& self_attr,
                   const MessageListener<MessageT>& listener);

    template <typename MessageT>
    void AddListener(const RoleAttributes& self_attr,
                   const RoleAttributes& opposite_attr,
                   const MessageListener<MessageT>& listener);


    template <typename MessageT>
    void RemoveListener(const RoleAttributes& self_attr);

    template <typename MessageT>
    void RemoveListener(const RoleAttributes& self_attr,
                      const RoleAttributes& opposite_attr);

    bool HasChannel(uint64_t channel_id);

protected:
    std::atomic<bool> is_shutdown_;
    //保存回调函数的哈希表，key为channel_id，值为此channel对应的ListenerHandler
    AtomicHashMap<uint64_t, ListenerHandlerBasePtr> msg_listeners_;
    base::AtomicRWLock rw_lock_;

};


template <typename MessageT>
void Dispatcher::AddListener(const RoleAttributes& self_attr,
                             const MessageListener<MessageT>& listener){
    if(is_shutdown_.load()){
        return ;
    }
    //拿到channel_id
    uint64_t channel_id = self_attr.channel_id;
    //创建一个新的ListenerHandler
    std::shared_ptr<ListenerHandler<MessageT>> handler;

    ListenerHandlerBasePtr* handler_base = nullptr;
    
    //如果此channel_id已经有ListenerHandler了
    if(msg_listeners_.Get(channel_id, &handler_base)){
        //取出此channel_id对应的ListenerHandler
        handler = 
            std::dynamic_pointer_cast<ListenerHandler<MessageT>>(*handler_base);
            if (handler == nullptr) {
      AERROR <<  "please ensure that readers with the same channel["
             << self_attr.channel_name
             << "] in the same process have the same message type";
      return;
             }
    } else{
        //说明此channel_id没有对应的ListenerHandler
        ADEBUG << "new reader for channel:"
           << GlobalData::GetChannelById(channel_id);
        //新建一个ListenerHandler
        handler.reset(new ListenerHandler<MessageT>());
        //建立channel_id 与 ListenerHandler的对应关系，保存到msg_listeners_中
        msg_listeners_.Set(channel_id, handler);
    }
    //为此ListenerHandler连接槽函数，一个id可以绑定多个槽函数
    handler->Connect(self_attr.id, listener);

}

template <typename MessageT>
void Dispatcher::AddListener(const RoleAttributes& self_attr,
                   const RoleAttributes& opposite_attr,
                   const MessageListener<MessageT>& listener)
{
  if (is_shutdown_.load()) {
    return;
  }
  uint64_t channel_id = self_attr.channel_id;
  std::shared_ptr<ListenerHandler<MessageT>> handler;

  ListenerHandlerBasePtr* handler_base = nullptr;
  if(msg_listeners_.Get(channel_id, &handler_base)){
        handler = 
            std::dynamic_pointer_cast<ListenerHandler<MessageT>>(*handler_base);
            if (handler == nullptr) {
      std::cout << "please ensure that readers with the same channel["
             << self_attr.channel_name
             << "] in the same process have the same message type"<< std::endl;
      return;
             }
    } else {
        std::cout << "new reader for channel:"
           << GlobalData::GetChannelById(channel_id);
        handler.reset(new ListenerHandler<MessageT>());
        msg_listeners_.Set(channel_id, handler);
    }

    handler->Connect(self_attr.id, opposite_attr.id, listener);
}

template <typename MessageT>
void Dispatcher::RemoveListener(const RoleAttributes& self_attr) {
  if (is_shutdown_.load()) {
    return;
  }
  uint64_t channel_id = self_attr.channel_id;

  ListenerHandlerBasePtr* handler_base = nullptr;
  if (msg_listeners_.Get(channel_id, &handler_base)) {
    (*handler_base)->Disconnect(self_attr.id);
  }
}

template <typename MessageT>
void Dispatcher::RemoveListener(const RoleAttributes& self_attr,
                                const RoleAttributes& opposite_attr) {
  if (is_shutdown_.load()) {
    return;
  }
  uint64_t channel_id = self_attr.channel_id;

  ListenerHandlerBasePtr* handler_base = nullptr;
  if (msg_listeners_.Get(channel_id, &handler_base)) {
    (*handler_base)->Disconnect(self_attr.id, opposite_attr.id);
  }
}



}
}
}

#endif