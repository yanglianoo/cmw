#ifndef CMW_NODE_PUBLISHER_H_
#define CMW_NODE_PUBLISHER_H_

#include <memory>
#include <cmw/node/publisher_base.h>
#include <cmw/transport/transport.h>
#include <cmw/discovery/topology_manager.h>
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

    void GetSubscriber(std::vector<RoleAttributes>* readers) override;

private:
    void JoinTheTopology();
    void LeaveTheTopology();
    void OnChannelChange(const ChangeMsg& change_msg);

    TransmitterPtr transmitter_;

    ChangeConnection change_conn_;

    


};

template<typename MessageT>
Publisher<MessageT>::Publisher(const RoleAttributes& role_attr)
    : PublisherBase(role_attr) , transmitter_(nullptr)  {}


}
}


#endif
