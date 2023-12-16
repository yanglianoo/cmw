#ifndef CMW_TRANSPORT_RECEIVER_RECEIVER_H_
#define CMW_TRANSPORT_RECEIVER_RECEIVER_H_

#include <functional>
#include <memory>
#include <cmw/transport/common/endpoint.h>
#include <cmw/transport/message/message_info.h>
namespace hnu    {
namespace cmw   {
namespace transport {


template <typename M>
class Receiver : public Endpoint{

public:
    using MessagePtr = std::shared_ptr<M>;
    using MessageListener = std::function<void(
        const MessagePtr&, const MessageInfo&, const RoleAttributes&)>;

    Receiver(const RoleAttributes& attr , const MessageListener& msg_listener);
    virtual ~Receiver();

    virtual void Enable();
    virtual void Disable();
    virtual void Enable(const RoleAttributes& opposite_attr) = 0;
    virtual void Disable(const RoleAttributes& opposite_attr) = 0;

protected:
    void OnNewMessage(const MessagePtr& msg, const MessageInfo& msg_info);

    MessageListener msg_listener_;

};

template <typename M>
Receiver<M>::Receiver(const RoleAttributes& attr,
                      const MessageListener& msg_listener)
    : Endpoint(attr), msg_listener_(msg_listener) {}

template <typename M>
Receiver<M>::~Receiver() {}

template <typename M>
void Receiver<M>::OnNewMessage(const MessagePtr& msg,
                               const MessageInfo& msg_info) {
  if (msg_listener_ != nullptr) {
    msg_listener_(msg, msg_info, attr_);
  }
}

}
}
}



#endif