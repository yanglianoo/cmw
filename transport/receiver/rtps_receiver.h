#ifndef CMW_TRANSPORT_RECEIVER_RTPS_RECEIVER_H_
#define CMW_TRANSPORT_RECEIVER_RTPS_RECEIVER_H_


#include <cmw/transport/receiver/receiver.h>
#include <cmw/transport/dispatcher/rtps_dispatcher.h>
namespace hnu    {
namespace cmw   {
namespace transport {

template <typename M>
class RtpsReceiver : public Receiver<M> {

public:
    RtpsReceiver(const RoleAttributes& attr,
               const typename Receiver<M>::MessageListener& msg_listener);
    virtual ~RtpsReceiver();
    void Enable() override;
    void Disable() override;
    void Enable(const RoleAttributes& opposite_attr) override;
    void Disable(const RoleAttributes& opposite_attr) override;
    
private:
    RtpsDispatcherPtr dispatcher_;
};


template <typename M>
RtpsReceiver<M>::RtpsReceiver(
    const RoleAttributes& attr,
    const typename Receiver<M>::MessageListener& msg_listener)
    : Receiver<M>(attr , msg_listener)
{
    dispatcher_ = RtpsDispatcher::Instance();
}

template <typename M>
RtpsReceiver<M>::~RtpsReceiver() {
  Disable();
}

template <typename M>
void RtpsReceiver<M>::Enable(){

    if(this->enabled_){
        return;
    }
    dispatcher_->AddListener<M>(
        this->attr_, std::bind(&RtpsReceiver<M>::OnNewMessage , this,
            std::placeholders::_1, std::placeholders::_2));

    this->enabled_ = true;
}

template <typename M>
void RtpsReceiver<M>::Disable() {
  if (!this->enabled_) {
    return;
  }
  dispatcher_->RemoveListener<M>(this->attr_);
  this->enabled_ = false;
}

template <typename M>
void RtpsReceiver<M>::Enable(const RoleAttributes& opposite_attr) {
  dispatcher_->AddListener<M>(
      this->attr_, opposite_attr,
      std::bind(&RtpsReceiver<M>::OnNewMessage, this, std::placeholders::_1,
                std::placeholders::_2));
}

template <typename M>
void RtpsReceiver<M>::Disable(const RoleAttributes& opposite_attr) {
  dispatcher_->RemoveListener<M>(this->attr_, opposite_attr);
}


}
}
}



#endif