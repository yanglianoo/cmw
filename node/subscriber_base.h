


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
#include <cmw/transport/receiver/receiver.h>
#include <cmw/data/data_dispatcher.h>

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

    /**
     * @brief Clear local data
     */
    virtual void ClearData() = 0;

    /**
     * @brief Get stored data
     */
    virtual void Observe() = 0;

    /**
     * @brief Query whether the Reader has data to be handled
     *
     * @return true if data container is empty
     * @return false if data container has data
     */
    virtual bool Empty() const = 0;   

    /**
     * @brief Query whether we have received data since last clear
     *
     * @return true if the reader has received data
     * @return false if the reader has not received data
     */
    virtual bool HasReceived() const = 0;

    /**
     * @brief Get time interval of since last receive message
     *
     * @return double seconds delay
     */
    virtual double GetDelaySec() const = 0;

    /**
     * @brief Get the value of pending queue size
     *
     * @return uint32_t result value
     */
    virtual uint32_t PendingQueueSize() const = 0;


    virtual bool HasPublisher() { return false; }

    virtual void GetPublishers(std::vector<RoleAttributes>* publishers) {}

    const std::string& GetChannelName() const {
        return role_attr_.channel_name;
    }

    uint64_t ChannelId() const { return role_attr_.channel_id; }

    const QosProfile& GetQosProfile() const {
        return role_attr_.qos_profile;
    }
    
    bool IsInit() const { return init_.load(); }
protected:

    RoleAttributes role_attr_;
    std::atomic<bool> init_;

};


template <typename MessageT>
class ReceiverManager{
    public:
        ~ReceiverManager() { receiver_map_.clear(); }
    
        auto GetReceiver(const RoleAttributes& role_attr) ->
                typename std::shared_ptr<transport::Receiver<MessageT>>;
    private:
        std::unordered_map<std::string, 
                    typename std::shared_ptr<transport::Receiver<MessageT>>> receiver_map_;
        std::mutex receiver_map_mutex_;
        DECLARE_SINGLETON(ReceiverManager<MessageT>)
};

template <typename MessageT>
ReceiverManager<MessageT>::ReceiverManager() {}

template <typename MessageT>
auto ReceiverManager<MessageT>::GetReceiver(const RoleAttributes& role_attr) ->
                typename std::shared_ptr<transport::Receiver<MessageT>>{
    
    std::lock_guard<std::mutex> lg(receiver_map_mutex_);
    const std::string& channel_name = role_attr.channel_name;
    //确保一个channel只有一个receiver
    if(receiver_map_.count(channel_name) == 0){
        receiver_map_[channel_name] = 
                transport::Transport::Instance()->CreateReceiver<MessageT>(
                    role_attr, [](const std::shared_ptr<MessageT>& msg,
                                  const transport::MessageInfo& msg_info,
                                  const RoleAttributes& subscriber_attr){
                            (void)msg_info;
                            (void)subscriber_attr;
                            data::DataDispatcher<MessageT>::Instance()->Dispatch(
                                subscriber_attr.channel_id, msg);
                            }
                );
    }
    return receiver_map_[channel_name];

}



}
}

#endif