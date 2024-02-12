#ifndef CMW_TRANSPORT_TRANSPORT_H_
#define CMW_TRANSPORT_TRANSPORT_H_


#include <cmw/transport/rtps/participant.h>
#include <cmw/common/macros.h>
#include <cmw/config/RoleAttributes.h>
#include <cmw/config/transport_config.h>
#include <cmw/transport/transmitter/transmitter.h>
#include <cmw/transport/transmitter/rtps_transmitter.h>
#include <cmw/transport/receiver/receiver.h>
#include <cmw/transport/dispatcher/rtps_dispatcher.h>
#include <cmw/transport/receiver/rtps_receiver.h>
#include <cmw/common/log.h>
#include <cmw/transport/transmitter/shm_transmitter.h>
#include <cmw/transport/receiver/shm_receiver.h>

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
    auto CreateTransmitter(const RoleAttributes& attr,
                           const OptionalMode& mode = OptionalMode::RTPS) ->
            typename std::shared_ptr<Transmitter<M>>;
    
    //返回一个Receiver的指针
    template <typename M>
    auto CreateReceiver(const RoleAttributes& attr,
                        const typename Receiver<M>::MessageListener& msg_listener,
                        const OptionalMode& mode = OptionalMode::RTPS) ->
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
auto Transport::CreateTransmitter(const RoleAttributes& attr,
                                  const OptionalMode& mode) ->
        typename std::shared_ptr<Transmitter<M>>
{
    if(is_shutdown_.load())
    {
        std::cout << "transport has been shut down." << std::endl;
        return nullptr;
    }
    std::shared_ptr<Transmitter<M>> transmitter = nullptr;
    RoleAttributes modified_attr = attr ;


    //默认走fast-rtps
    switch (mode)
    {
        case OptionalMode::SHM: 
            transmitter = std::make_shared<ShmTransmitter<M>>(modified_attr);
            break;
        default:
            transmitter = std::make_shared<RtpsTransmitter<M>>(modified_attr , participant());
            break;
    }

    RETURN_VAL_IF_NULL(transmitter, nullptr);

    if( mode != OptionalMode::HYBRID){
        ADEBUG << "transmitter Enable";
        transmitter->Enable();
    }
    AINFO << "CreateTransmitter Sucess";
    return transmitter;
}

template <typename M>
auto Transport::CreateReceiver(const RoleAttributes& attr,
                               const typename Receiver<M>::MessageListener& msg_listener,
                               const OptionalMode& mode ) ->
                               typename std::shared_ptr<Receiver<M>>
{
    if(is_shutdown_.load()){
        AINFO << "transport has been shut down.";
        return nullptr;
    }

    //新建一个Receiver<M>类型的共享指针
    std::shared_ptr<Receiver<M>> receiver = nullptr;

    RoleAttributes modified_attr = attr;
    switch (mode)
    {
        case OptionalMode::SHM:
            receiver = std::make_shared<ShmReceiver<M>>(modified_attr, msg_listener);
        default:
            receiver = std::make_shared<RtpsReceiver<M>>(modified_attr , msg_listener);
            break;
    }

    //保证receiver不为空
    RETURN_VAL_IF_NULL(receiver, nullptr);

    if (mode != OptionalMode::HYBRID) {
        receiver->Enable();
    }
    return receiver;
}

}
}
}

#endif
