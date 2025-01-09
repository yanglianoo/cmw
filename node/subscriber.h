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
#include <cmw/blocker/blocker.h>
#include <cmw/scheduler/scheduler_factory.h>
#include <cmw/croutine/croutine_factory.h>

namespace hnu    {
namespace cmw   {


template <typename M0>
using CallbackFunc = std::function<void(const std::shared_ptr<M0>&)>;

const uint32_t DEFAULT_PENDING_QUEUE_SIZE = 1;

template <typename MessageT>
class Subscriber : public SubscriberBase {

public:
    using BlockerPtr = std::unique_ptr<blocker::Blocker<MessageT>>;
    using ReceiverPtr = std::shared_ptr<transport::Receiver<MessageT>>;
    using ChangeConnection = 
            typename discovery::Manager::ChangeConnection;
    using Iterator =
            typename std::list<std::shared_ptr<MessageT>>::const_iterator;

    explicit Subscriber(const RoleAttributes& role_attr,
                        const CallbackFunc<MessageT>& subscriber_func = nullptr,
                        uint32_t pending_queue_size = DEFAULT_PENDING_QUEUE_SIZE);
    virtual ~Subscriber();
    bool Init() override;
    void Shutdown() override;

    /**
     * @brief Get All data that `Blocker` stores
     */
    void Observe() override;

    /**
     * @brief Clear `Blocker`'s data
     */
    void ClearData() override;

    /**
     * @brief Query whether we have received data since last clear
     *
     * @return true if the reader has received data
     * @return false if the reader has not received data
     */
    bool HasReceived() const override;

    /**
     * @brief Query whether the Reader has data to be handled
     *
     * @return true if blocker is empty
     * @return false if blocker has data
     */
    bool Empty() const override;

    /**
     * @brief Get pending_queue_size configuration
     *
     * @return uint32_t the value of pending queue size
     */
    uint32_t PendingQueueSize() const override;

    /**
     * @brief Get time interval of since last receive message
     *
     * @return double seconds delay
     */
    double GetDelaySec() const override;

    /**
     * @brief Push `msg` to Blocker's `PublishQueue`
     *
     * @param msg message ptr to be pushed
     */
    virtual void Enqueue(const std::shared_ptr<MessageT>& msg);

    /**
     * @brief Set Blocker's `PublishQueue`'s capacity to `depth`
     *
     * @param depth the value you  want to set
     */
    virtual void SetHistoryDepth(const uint32_t& depth);

    /**
     * @brief Get Blocker's `PublishQueue`'s capacity
     *
     * @return uint32_t depth of the history
     */
    virtual uint32_t GetHistoryDepth() const;

    /**
     * @brief Get the begin iterator of `ObserveQueue`, used to traverse
     *
     * @return Iterator begin iterator
     */
    virtual Iterator Begin() const { return blocker_->ObservedBegin(); }

    /**
     * @brief Get the end iterator of `ObserveQueue`, used to traverse
     *
     * @return Iterator begin iterator
     */
    virtual Iterator End() const { return blocker_->ObservedEnd(); }

    /**
     * @brief Get the latest message we `Observe`
     *
     * @return std::shared_ptr<MessageT> the latest message
     */
    virtual std::shared_ptr<MessageT> GetLatestObserved() const;

    /**
     * @brief Get the oldest message we `Observe`
     *
     * @return std::shared_ptr<MessageT> the oldest message
     */
    virtual std::shared_ptr<MessageT> GetOldestObserved() const;

protected:
    double latest_recv_time_sec_ = -1.0;
    double second_to_lastest_recv_time_sec_ = -1.0;
    uint32_t pending_queue_size_;

private:
    void JoinTheTopology();
    void LeaveTheTopology();
    void OnChannelChange(const ChangeMsg& change_msg);

    CallbackFunc<MessageT> subscriber_func_;
    ReceiverPtr receiver_ = nullptr;
    std::string croutine_name_;

    BlockerPtr blocker_ = nullptr;

    ChangeConnection change_conn_;
    discovery::ChannelManagerPtr channel_manager_ = nullptr;

};

template <typename MessageT>
Subscriber<MessageT>::Subscriber(const RoleAttributes& role_attr,
                        const CallbackFunc<MessageT>& subscriber_func,
                        uint32_t pending_queue_size)
                        : SubscriberBase(role_attr) ,
                          subscriber_func_(subscriber_func),
                          pending_queue_size_(pending_queue_size){ 
    //新建一个Blocker
    blocker_.reset(new blocker::Blocker<MessageT>(blocker::BlockerAttr(
        role_attr.qos_profile.depth, role_attr.channel_name)));
}

template <typename MessageT>
Subscriber<MessageT>::~Subscriber(){
    Shutdown();
}

template <typename MessageT>
void Subscriber<MessageT>::Enqueue(const std::shared_ptr<MessageT>& msg){
    second_to_lastest_recv_time_sec_ = latest_recv_time_sec_;
    latest_recv_time_sec_ = Time::Now().ToSecond();
    blocker_->Publish(msg);
}

template <typename MessageT>
void Subscriber<MessageT>::Observe(){
    blocker_->Observe();
}


template <typename MessageT>
bool Subscriber<MessageT>::Init(){
    if(init_.exchange(true)){
        return true;
    }
    std::function<void(const std::shared_ptr<MessageT>&)> func;
    if(subscriber_func_ != nullptr){
        func =   [this](const std::shared_ptr<MessageT>& msg){
                        this->Enqueue(msg);
                        this->subscriber_func_(msg);
                       };

    } else {
        func = [this](const std::shared_ptr<MessageT>& msg) { this->Enqueue(msg);};
    }

    auto sched = scheduler::Instance();
    croutine_name_ = role_attr_.node_name + "_" + role_attr_.channel_name;

    //创建一个DataVisitor
    auto dv = std::make_shared<data::DataVisitor<MessageT>>(
        role_attr_.channel_id, pending_queue_size_);

    // Using factory to wrap templates.
    croutine::RoutineFactory factory =
      croutine::CreateRoutineFactory<MessageT>(std::move(func), dv);
    
    if(!sched->CreateTask(factory, croutine_name_)){
        AERROR << "Create Task Failed!";
        init_.store(false);
        return false;
    }

    receiver_ = ReceiverManager<MessageT>::Instance()->GetReceiver(role_attr_);

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

    if(!croutine_name_.empty()){
        scheduler::Instance()->RemoveTask(croutine_name_);
    }
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

template <typename MessageT>
bool Subscriber<MessageT>::HasReceived() const {
  return !blocker_->IsPublishedEmpty();
}

template <typename MessageT>
bool Subscriber<MessageT>::Empty() const {
  return blocker_->IsObservedEmpty();
}


template <typename MessageT>
uint32_t Subscriber<MessageT>::PendingQueueSize() const {
  return pending_queue_size_;
}

template <typename MessageT>
double Subscriber<MessageT>::GetDelaySec() const {
    if (latest_recv_time_sec_ < 0) {
        return -1.0;
    }   

    if(second_to_lastest_recv_time_sec_ < 0){
        return Time::Now().ToSecond() - latest_recv_time_sec_;
    }

    return std::max((Time::Now().ToSecond() - latest_recv_time_sec_),
                        (latest_recv_time_sec_ - second_to_lastest_recv_time_sec_));
}

template <typename MessageT>
void Subscriber<MessageT>::ClearData() {
    blocker_->ClearPublished();
    blocker_->ClearObserved();
}

template <typename MessageT>
void Subscriber<MessageT>::SetHistoryDepth(const uint32_t& depth) {
  blocker_->set_capacity(depth);
}

template <typename MessageT>
uint32_t Subscriber<MessageT>::GetHistoryDepth() const {
  return static_cast<uint32_t>(blocker_->capacity());
}

template <typename MessageT>
std::shared_ptr<MessageT> Subscriber<MessageT>::GetLatestObserved() const {
  return blocker_->GetLatestObservedPtr();
}

template <typename MessageT>
std::shared_ptr<MessageT> Subscriber<MessageT>::GetOldestObserved() const {
  return blocker_->GetOldestObservedPtr();
}

}
}

#endif