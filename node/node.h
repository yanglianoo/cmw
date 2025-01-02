#ifndef CMW_NODE_NODE_H_
#define CMW_NODE_NODE_H_


#include <map>
#include <memory>
#include <string>
#include <utility>

#include <cmw/node/node_channel_impl.h>

namespace hnu    {
namespace cmw   {


class Node {

public:
    friend bool Init(const char*);
    friend std::unique_ptr<Node> CreateNode(const std::string&,
                                            const std::string&);    
    virtual ~Node();

    /**
     * @brief Get node's name.
     * @warning duplicate node name is not allowed in the topo.
     */
    const std::string& Name() const;

    template <typename MessageT>
    auto CreatePublisher(const RoleAttributes& role_attr)
        -> std::shared_ptr<Publisher<MessageT>>;
    
    template <typename MessageT>
    auto CreatePublisher(const std::string& channel_name)
        -> std::shared_ptr<Publisher<MessageT>>;

    template <typename MessageT>
    auto CreateSubscriber(const std::string& channel_name,
                        const CallbackFunc<MessageT>& reader_func = nullptr)
        -> std::shared_ptr<Subscriber<MessageT>>;  

    template <typename MessageT>
    auto CreateSubscriber(const SubscriberConfig& config,
                        const CallbackFunc<MessageT>& reader_func = nullptr)
        -> std::shared_ptr<Subscriber<MessageT>>;     

    template <typename MessageT>
    auto CreateSubscriber(const RoleAttributes& role_attr,
                        const CallbackFunc<MessageT>& reader_func = nullptr)
        -> std::shared_ptr<Subscriber<MessageT>>;    

    /**
     * @brief Observe all readers' data
     */
    void Observe();

    /**
     * @brief clear all readers' data
     */
    void ClearData();

    template <typename MessageT>
    auto GetSubscriber(const std::string& channel_name)
        -> std::shared_ptr<Subscriber<MessageT>>;
    

private:

    explicit Node(const std::string& node_name,
                    const std::string& name_space = "");

    std::string node_name_;
    std::string name_space_;

    std::mutex subscribers_mutex_;
    std::map<std::string, std::shared_ptr<SubscriberBase>> subscribers_;

    std::unique_ptr<NodeChannelImpl> node_channel_impl_ = nullptr;
};

template <typename MessageT>
auto Node::CreatePublisher(const RoleAttributes& role_attr)
        -> std::shared_ptr<Publisher<MessageT>>{
    return node_channel_impl_->template CreatePublisher<MessageT>(role_attr);
}

template <typename MessageT>
auto Node::CreatePublisher(const std::string& channel_name)
        -> std::shared_ptr<Publisher<MessageT>>{
    return node_channel_impl_->template  CreatePublisher<MessageT>(channel_name);     
}

template <typename MessageT>
auto Node::CreateSubscriber(const RoleAttributes& role_attr,
                      const CallbackFunc<MessageT>& reader_func)
        -> std::shared_ptr<Subscriber<MessageT>>{
    std::lock_guard<std::mutex> lg(subscribers_mutex_);
    if(subscribers_.find(role_attr.channel_name) != subscribers_.end()){
        AWARN << "Failed to create subscriber: subscriber with the same channel already "
                "exists."; 
        return nullptr;       
    }

    auto subscriber = node_channel_impl_->template CreateSubscriber<MessageT>(
                         role_attr,reader_func);
    if(subscriber != nullptr){
        subscribers_.emplace(std::make_pair(role_attr.channel_name, subscriber));
    }
    return subscriber;
} 

template <typename MessageT>
auto Node::CreateSubscriber(const std::string& channel_name,
                        const CallbackFunc<MessageT>& reader_func)
        -> std::shared_ptr<Subscriber<MessageT>>{
    std::lock_guard<std::mutex> lg(subscribers_mutex_);
    if(subscribers_.find(channel_name) != subscribers_.end()){
        AWARN << "Failed to create subscriber: subscriber with the same channel already "
                "exists."; 
        return nullptr;       
    }

    auto subscriber = node_channel_impl_->template CreateSubscriber<MessageT>(
                         channel_name, reader_func);
    
    if(subscriber != nullptr){
        subscribers_.emplace(std::make_pair(channel_name, subscriber));
    }
    return subscriber;
}

template <typename MessageT>
auto Node::CreateSubscriber(const SubscriberConfig& config,
                        const CallbackFunc<MessageT>& reader_func)
        -> std::shared_ptr<Subscriber<MessageT>>{
    std::lock_guard<std::mutex> lg(subscribers_mutex_);
    if(subscribers_.find(config.channel_name) != subscribers_.end()){
        AWARN << "Failed to create subscriber: subscriber with the same channel already "
                "exists."; 
        return nullptr;       
    }

    auto subscriber = node_channel_impl_->template CreateSubscriber<MessageT>(
                         config, reader_func);

    if(subscriber != nullptr){
        subscribers_.emplace(std::make_pair(config.channel_name, subscriber));
    }
    return subscriber;
}

template <typename MessageT>
auto Node::GetSubscriber(const std::string& channel_name)
        -> std::shared_ptr<Subscriber<MessageT>>{
    std::lock_guard<std::mutex> lg(subscribers_mutex_);
    auto it = subscribers_.find(channel_name);
    if(it != subscribers_.end()){
        return std::dynamic_pointer_cast<Subscriber<MessageT>>(it->second);
    }  
    return nullptr;
}


}
}


#endif