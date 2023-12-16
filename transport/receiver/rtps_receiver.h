#ifndef CMW_TRANSPORT_RECEIVER_RTPS_RECEIVER_H_
#define CMW_TRANSPORT_RECEIVER_RTPS_RECEIVER_H_


#include <cmw/transport/receiver/receiver.h>
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
    
};

}
}
}



#endif