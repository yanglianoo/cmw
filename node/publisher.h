#ifndef CMW_NODE_PUBLISHER_H_
#define CMW_NODE_PUBLISHER_H_

#include <memory>
#include <cmw/node/publisher_base.h>
#include <cmw/transport/transport.h>
#include <cmw/discovery/topology_manager.h>
#include <cmw/transport/transmitter/transmitter.h>
#include <cmw/transport/transmitter/rtps_transmitter.h>
#include <cmw/common/log.h>
namespace hnu    {
namespace cmw   {

template<typename MessageT>
class Publisher : public PublisherBase
{
public:
    using TransmitterPtr = std::shared_ptr<transport::Transmitter<MessageT>>;
    using ChangeConnection = typename discovery::Manager::ChangeConnection;

    explicit Publisher(const RoleAttributes& role_attr);
    virtual ~Publisher();


    bool Init() override;

    void Shutdown() override;

    bool HasSubscriber() override;

    void GetSubscribers(std::vector<RoleAttributes>* subscribers) override;

    virtual bool Publish(const MessageT& msg);
    virtual bool Publish(const std::shared_ptr<MessageT>& msg_ptr);

private:
    void JoinTheTopology();
    void LeaveTheTopology();
    void OnChannelChange(const ChangeMsg& change_msg);

    TransmitterPtr transmitter_;

    ChangeConnection change_conn_;

    discovery::ChannelManagerPtr channel_manager_;

};

template<typename MessageT>
Publisher<MessageT>::Publisher(const RoleAttributes& role_attr)
    : PublisherBase(role_attr) , transmitter_(nullptr) , channel_manager_(nullptr){}

template<typename MessageT>
Publisher<MessageT>::~Publisher(){
    Shutdown();
}

template<typename MessageT>
bool Publisher<MessageT>::Init(){
    {
        std::lock_guard<std::mutex> lg(lock_);
        if(init_){
            return true;
        }
        transmitter_ = transport::Transport::Instance()->CreateTransmitter<MessageT>(role_attr_);
        if(transmitter_ == nullptr){
            return false;
        }
        init_ = true;
    }
    this->role_attr_.id = transmitter_->id().HashValue();
    channel_manager_ = 
        discovery::TopologyManager::Instance()->channel_manager();
    JoinTheTopology();
    return true;
}

template<typename MessageT>
void Publisher<MessageT>::Shutdown(){
    {
        std::lock_guard<std::mutex> lg(lock_);
        if(!init_){
            return;
        }
        init_ = false;
    }
    LeaveTheTopology();
    transmitter_ = nullptr;
    channel_manager_ = nullptr;
}

template<typename MessageT>
bool Publisher<MessageT>::Publish(const MessageT& msg){
    RETURN_VAL_IF(!PublisherBase::IsInit() , false);
    auto msg_ptr = std::make_shared<MessageT>(msg);
    return Publish(msg_ptr);
}

template<typename MessageT>
bool Publisher<MessageT>::Publish(const std::shared_ptr<MessageT>& msg_ptr){
    RETURN_VAL_IF(!PublisherBase::IsInit(), false);
    transmitter_->Transmit(msg_ptr);
}

template<typename MessageT>
void Publisher<MessageT>::JoinTheTopology(){

    //
    change_conn_ = channel_manager_->AddChangeListener(std::bind(
        &Publisher<MessageT>::OnChannelChange, this , std::placeholders::_1));
    
    const std::string& channel_name = this->role_attr_.channel_name;
    std::vector<RoleAttributes> subscribers;
    channel_manager_->GetReadersOfChannel(channel_name , &subscribers);

    for(auto& subscriber : subscribers){
        transmitter_->Enable(subscriber);
    }

    //加入拓扑图
    channel_manager_->Join(this->role_attr_, RoleType::ROLE_WRITER);
}

template<typename MessageT>
void Publisher<MessageT>::LeaveTheTopology(){
    channel_manager_->RemoveChangeListener(change_conn_);
    channel_manager_->Leave(this->role_attr_, RoleType::ROLE_WRITER);
}

template<typename MessageT>
void Publisher<MessageT>::OnChannelChange(const ChangeMsg& change_msg){
    
    if(change_msg.role_type != RoleType::ROLE_READER){
        return;
    }

    auto& subscriber_attr = change_msg.role_attr;
    if(subscriber_attr.channel_name != this->role_attr_.channel_name){
        return;
    }

    auto operate_type = change_msg.operate_type;
    if(operate_type == OperateType::OPT_JOIN){
        //确定有新的关注channel_name 的 reader 加入时 transmitter_ 才会 enable
        transmitter_->Enable(subscriber_attr);
    } else {
        transmitter_->Disable(subscriber_attr);
    }

}

template<typename MessageT>
bool Publisher<MessageT>::HasSubscriber(){
    RETURN_VAL_IF(!PublisherBase::IsInit(), false);
    return channel_manager_->HasReader(role_attr_.channel_name);
}

template<typename MessageT>
void Publisher<MessageT>::GetSubscribers(std::vector<RoleAttributes>* subscribers){
    if(subscribers == nullptr){
        return;
    }

    if(!PublisherBase::IsInit()){
        return;
    }

    channel_manager_->GetReadersOfChannel(role_attr_.channel_name , subscribers);
}

}
}


#endif
