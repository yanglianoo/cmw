#ifndef CMW_NODE_WRITER_H_
#define CMW_NODE_WRITER_H_

#include <memory>
#include <string>
#include <vector>

#include <cmw/node/subscriber_base.h>
#include <cmw/transport/transport.h>
#include <cmw/transport/receiver/receiver.h>
#include <cmw/discovery/specific_manager/manager.h>
#include <cmw/discovery/specific_manager/channel_manager.h>
#include <cmw/discovery/topology_manager.h>

namespace hnu    {
namespace cmw   {


template <typename M0>
using CallbackFunc = std::function<void(const std::shared_ptr<M0>&)>;

template <typename MessageT>
class Subscriber : public SubscriberBase {

public:
    using ReceiverPtr = std::shared_ptr<transport::Receiver<MessageT>>;
    using ChangeConnection = 
            typename discovery::Manager::ChangeConnection;

    explicit Subscriber(const RoleAttributes& role_attr,
                        const CallbackFunc<MessageT>& subscriber_func = nullptr);
    virtual ~Subscriber();
    bool Init() override;
    void Shutdown() override;
   // bool HasPublisher() override;
   // bool GetPublishers(std::vector<RoleAttributes>* publishers)  override;
private:
    void JoinTheTopology();
    void LeaveTheTopology();
    void OnChannelChange(const ChangeMsg& change_msg);
    CallbackFunc<MessageT> subscriber_func_;
    ReceiverPtr receiver_ = nullptr;

    ChangeConnection change_conn_;
    discovery::ChannelManagerPtr channel_manager_ = nullptr;

};

template <typename MessageT>
Subscriber<MessageT>::Subscriber(const RoleAttributes& role_attr,
                        const CallbackFunc<MessageT>& subscriber_func )
                        : SubscriberBase(role_attr) ,
                          subscriber_func_(subscriber_func) { }

template <typename MessageT>
Subscriber<MessageT>::~Subscriber(){
    Shutdown();
}

template <typename MessageT>
bool Subscriber<MessageT>::Init(){
    if(init_.exchange(true)){
        return true;
    }
    if(subscriber_func_ != nullptr){
        
        auto func =   [this](const std::shared_ptr<MessageT>& message ,
                       const transport::MessageInfo&, const RoleAttributes&){
                        subscriber_func_(message);
                       };

        receiver_ = transport::Transport::Instance()->CreateReceiver<MessageT>(role_attr_,func);
    }

    channel_manager_ = discovery::TopologyManager::Instance()->channel_manager();

    JoinTheTopology();

    return true;
}

template <typename MessageT>
void Subscriber<MessageT>::Shutdown(){
    if(!init_.exchange(false)){
        return;
    }
    LeaveTheTopology();
    receiver_ = nullptr;
    channel_manager_ = nullptr;
}

template <typename MessageT>
void Subscriber<MessageT>::JoinTheTopology(){
    change_conn_ = channel_manager_->AddChangeListener(std::bind(
        &Subscriber<MessageT>::OnChannelChange , this , std::placeholders::_1));
    
    const std::string& channel_name = this->role_attr_.channel_name;
    std::vector<RoleAttributes> publishers;
    channel_manager_->GetWritersOfChannel(channel_name, &publishers);
    for (auto& publisher : publishers){
        receiver_->Enable(publisher);
    }
    channel_manager_->Join(this->role_attr_ , RoleType::ROLE_READER);
}

template <typename MessageT>
void Subscriber<MessageT>::LeaveTheTopology(){
    channel_manager_->RemoveChangeListener(change_conn_);
    channel_manager_->Leave(this->role_attr_ , RoleType::ROLE_READER);
}

template <typename MessageT>
void Subscriber<MessageT>::OnChannelChange(const ChangeMsg& change_msg){
    if(change_msg.role_type != RoleType::ROLE_WRITER){
        return;
    }

    auto& publisher_attr = change_msg.role_attr;
    if(publisher_attr.channel_name != this->role_attr_.channel_name)
    {
        return;
    }

    auto operate_type = change_msg.operate_type;
    if(operate_type == OperateType::OPT_JOIN){
        receiver_->Enable(publisher_attr);
    } else {
        receiver_->Disable(publisher_attr);
    }
}



}
}

#endif