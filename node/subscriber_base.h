


#ifndef CMW_NODE_SUBSCRIBER_BASE_H_
#define CMW_NODE_SUBSCRIBER_BASE_H_

#include <atomic>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>


#include <cmw/config/RoleAttributes.h>
#include <cmw/common/macros.h>
#include <cmw/event/perf_event_cache.h>
#include <cmw/transport/transport.h>

namespace hnu    {
namespace cmw   {

using namespace config;
class SubscriberBase
{
public:
    explicit SubscriberBase(const RoleAttributes& role_attr )
                : role_attr_(role_attr) , init_(false) {}
    virtual ~SubscriberBase() {}

    virtual bool Init() = 0;

    virtual void Shutdown() = 0;

    virtual void ClearData() = 0;

    virtual void Observe() = 0;

    virtual bool Empty() const = 0;

    virtual bool HasReceived() const = 0;

    virtual double GetDelaySec() const = 0;

    virtual uint32_t PendingQueueSize() const = 0;

    virtual bool HasPublisher() { return false; }

    virtual void GetPublishers(std::vector<RoleAttributes>* publishers) {}

    const std::string& GetChannelName() const {
        return role_attr_.channel_name;
    }

    uint64_t ChannelId() const { return role_attr_.channel_id; }

    const QosProfile& QosProfile() const {
        return role_attr_.qos_profile;
    }
    
    bool IsInit() const { return init_.load(); }
protected:

    RoleAttributes role_attr_;
    std::atomic<bool> init_;

};




}
}

#endif