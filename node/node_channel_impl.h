#ifndef CMW_NODE_NODE_CHANNEL_IMPL_H_
#define CMW_NODE_NODE_CHANNEL_IMPL_H_

#include <memory>
#include <string>

#include <cmw/common/global_data.h>
#include <cmw/node/subscriber.h>
#include <cmw/node/publisher.h>

namespace hnu    {
namespace cmw   {

class Node;

struct SubscriberConfig
{
    SubscriberConfig(){
        qos_profile.history = QosHistoryPolicy::HISTORY_KEEP_LAST;
        qos_profile.depth = 1;
        qos_profile.mps = 0;
        qos_profile.reliability = QosReliabilityPolicy::RELIABILITY_RELIABLE;
        qos_profile.durability = QosDurabilityPolicy::DURABILITY_VOLATILE;
    }

    SubscriberConfig(const SubscriberConfig& other)
        : channel_name(other.channel_name),
          qos_profile(other.qos_profile),
          pending_queue_size(other.pending_queue_size) {}

    std::string channel_name;
    QosProfile qos_profile;
    /**
     * @brief configuration for responding ChannelBuffer.
     * Older messages will dropped if you have no time to handle
     */
    uint32_t pending_queue_size;
};


class NodeChannelImpl {
    friend class Node;

public:
    using NodeManagerPtr = std::shared_ptr<discovery::NodeManager>;

    /**
     * @brief Construct a new Node Channel Impl object
     *
     * @param node_name node name
     */
    explicit NodeChannelImpl(const std::string& node_name)
        : is_reality_mode_(true), node_name_(node_name){
        
        node_attr_.host_name = common::GlobalData::Instance()->HostName();
        node_attr_.host_ip = common::GlobalData::Instance()->HostIp();
        node_attr_.process_id = common::GlobalData::Instance()->ProcessId();
        node_attr_.node_name = node_name;
        uint64_t node_id = common::GlobalData::RegisterNode(node_name);
        node_attr_.node_id = node_id;

        if(is_reality_mode_) {
            node_manager_ = discovery::TopologyManager::Instance()->node_manager();
            node_manager_->Join(node_attr_, RoleType::ROLE_NODE);
        }
    }

    /**
     * @brief Destroy the Node Channel Impl object
     */
    virtual ~NodeChannelImpl() {
        if (is_reality_mode_) {
        node_manager_->Leave(node_attr_, RoleType::ROLE_NODE);
        node_manager_ = nullptr;
        }
    }

    /**
     * @brief get name of this node
     *
     * @return const std::string& actual node name
     */
    const std::string& NodeName() const { return node_name_; }

private:

    template <typename MessageT>
    auto CreatePublisher(const RoleAttributes& role_attr)
        -> std::shared_ptr<Publisher<MessageT>>;
    
    template <typename MessageT>
    auto CreatePublisher(const std::string& channel_name)
        -> std::shared_ptr<Publisher<MessageT>>;
    
    template <typename MessageT>
    auto CreateSubscriber(const std::string& channel_name,
                        const CallbackFunc<MessageT>& reader_func)
        -> std::shared_ptr<Subscriber<MessageT>>;

    template <typename MessageT>
    auto CreateSubscriber(const SubscriberConfig& config,
                        const CallbackFunc<MessageT>& reader_func)
        -> std::shared_ptr<Subscriber<MessageT>>;

    template <typename MessageT>
    auto CreateSubscriber(const RoleAttributes& role_attr,
                        const CallbackFunc<MessageT>& reader_func,
                        uint32_t pending_queue_size = DEFAULT_PENDING_QUEUE_SIZE)
        -> std::shared_ptr<Subscriber<MessageT>>;

    template <typename MessageT>
    auto CreateSubscriber(const RoleAttributes& role_attr)
        -> std::shared_ptr<Subscriber<MessageT>>;

    template <typename MessageT>
    void FillInAttr(RoleAttributes* attr);

    bool is_reality_mode_;
    std::string node_name_;
    RoleAttributes node_attr_;
    NodeManagerPtr node_manager_ = nullptr;

};

template <typename MessageT>
auto NodeChannelImpl::CreatePublisher(const RoleAttributes& role_attr)
        -> std::shared_ptr<Publisher<MessageT>> {
    
    if(role_attr.channel_name.empty()){
        AERROR << "Can't create a writer with empty channel name!";
        return nullptr;
    }

    RoleAttributes new_attr(role_attr);
    FillInAttr<MessageT>(&new_attr);

    std::shared_ptr<Publisher<MessageT>> publisher_ptr = nullptr;
    
    publisher_ptr = std::make_shared<Publisher<MessageT>>(new_attr);

    RETURN_VAL_IF_NULL(publisher_ptr, nullptr);
    RETURN_VAL_IF(!publisher_ptr->Init(), nullptr);

    return publisher_ptr;
}

template <typename MessageT>
auto NodeChannelImpl::CreatePublisher(const std::string& channel_name)
        -> std::shared_ptr<Publisher<MessageT>>{
    RoleAttributes role_attr;
    role_attr.channel_name = channel_name;
    return this->CreatePublisher<MessageT>(role_attr);
}

template <typename MessageT>
auto NodeChannelImpl::CreateSubscriber(const std::string& channel_name,
                        const CallbackFunc<MessageT>& reader_func)
        -> std::shared_ptr<Subscriber<MessageT>>{
    RoleAttributes role_attr;
    role_attr.channel_name = channel_name;
    return this->template CreateSubscriber<MessageT>(role_attr, reader_func);
}

template <typename MessageT>
auto NodeChannelImpl::CreateSubscriber(const SubscriberConfig& config,
                                   const CallbackFunc<MessageT>& reader_func)
        -> std::shared_ptr<Subscriber<MessageT>> {
    
    RoleAttributes role_attr;
    role_attr.channel_name = config.channel_name;
    role_attr.qos_profile = config.qos_profile;
    return this->template CreateSubscriber<MessageT>(role_attr, reader_func,
                                               config.pending_queue_size);
}

template <typename MessageT>
auto NodeChannelImpl::CreateSubscriber(const RoleAttributes& role_attr,
                                   const CallbackFunc<MessageT>& reader_func,
                                   uint32_t pending_queue_size)
        -> std::shared_ptr<Subscriber<MessageT>> {
    
    if(role_attr.channel_name.empty()){
        AERROR << "Can't create a reader with empty channel name!";
        return nullptr;
    }

    RoleAttributes new_attr(role_attr);
    FillInAttr<MessageT>(&new_attr);

    std::shared_ptr<Subscriber<MessageT>> subscriber_ptr = nullptr;

    subscriber_ptr = std::make_shared<Subscriber<MessageT>>(new_attr, reader_func,
                                                            pending_queue_size);
    RETURN_VAL_IF_NULL(subscriber_ptr, nullptr);
    RETURN_VAL_IF(!subscriber_ptr->Init(), nullptr);
    return subscriber_ptr;
}


template <typename MessageT>
void NodeChannelImpl::FillInAttr(RoleAttributes* attr){
    attr->host_name = node_attr_.host_name;
    attr->host_ip = node_attr_.host_ip;
    attr->process_id = node_attr_.process_id;
    attr->node_name = node_attr_.node_id;
    attr->node_id = node_attr_.node_id;

    auto channel_id = common::GlobalData::RegisterChannel(attr->channel_name);
    attr->channel_id = channel_id;
}


}
}

#endif