
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
namespace hnu    {
namespace cmw   {
namespace transport {


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
    


};

}
}
}

#endif