#ifndef CMW_TRANSPORT_TRANSPORT_H_
#define CMW_TRANSPORT_TRANSPORT_H_


#include <cmw/transport/rtps/participant.h>
#include <cmw/common/macros.h>
#include <cmw/config/RoleAttributes.h>
#include <cmw/transport/transmitter/transmitter.h>
#include <cmw/transport/transmitter/rtps_transmitter.h>
#include <cmw/transport/receiver/receiver.h>
#include <cmw/transport/dispatcher/rtps_dispatcher.h>
#include <cmw/transport/receiver/rtps_receiver.h>

namespace hnu    {
namespace cmw   {
namespace transport {

using namespace config;

/**
 * @brief  Transport 构造时会创建 participant，然后通过CreateTransmitter 将此participant作为参数传递用于创建RtpsTransmitte
 */
class Transport
{
public:
    virtual ~Transport();

    void Shutdown();

    //返回一个Transmitter的指针
    template <typename M>
    auto CreateTransmitter(const RoleAttributes& attr) ->
            typename std::shared_ptr<Transmitter<M>>;
    
    //返回一个Receiver的指针
    template <typename M>
    auto CreateReceiver(const RoleAttributes& attr,
                        const typename Receiver<M>::MessageListener& msg_listener) ->
            typename std::shared_ptr<Receiver<M>>;

    //返回在构造函数中创建的participant_
    ParticipantPtr participant() const { return participant_; }
private:
    //构造函数会调用，来创建一个participant
    void CreateParticipant();
    ParticipantPtr participant_ = nullptr;
    std::atomic<bool> is_shutdown_ = {false};

    RtpsDispatcherPtr rtps_dispatcher_ = nullptr;

    DECLARE_SINGLETON(Transport)
};


template <typename M>
auto Transport::CreateTransmitter(const RoleAttributes& attr) ->
        typename std::shared_ptr<Transmitter<M>>
{
    if(is_shutdown_.load())
    {
        std::cout << "transport has been shut down." << std::endl;
        return nullptr;
    }
    std::shared_ptr<Transmitter<M>> transmitter = nullptr;
    RoleAttributes modified_attr = attr ;

    transmitter = std::make_shared<RtpsTransmitter<M>>(modified_attr , participant());

    transmitter->Enable();

    return transmitter;

}

template <typename M>
auto Transport::CreateReceiver(const RoleAttributes& attr,
                               const typename Receiver<M>::MessageListener& msg_listener) ->
                               typename std::shared_ptr<Receiver<M>>
{
    if(is_shutdown_.load()){
        std::cout << "transport has been shut down."<< std::endl;
        return nullptr;
    }

    std::shared_ptr<Receiver<M>> receiver = nullptr;
    RoleAttributes modified_attr = attr;

    receiver = std::make_shared<RtpsReceiver<M>>(modified_attr , msg_listener);

    receiver->Enable();

    return receiver;
}

}
}
}

#endif
