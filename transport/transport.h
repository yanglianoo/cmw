#ifndef CMW_TRANSPORT_TRANSPORT_H_
#define CMW_TRANSPORT_TRANSPORT_H_


#include <cmw/transport/rtps/participant.h>
#include <cmw/common/macros.h>
#include <cmw/config/RoleAttributes.h>
#include <cmw/transport/transmitter/transmitter.h>
#include <cmw/transport/transmitter/rtps_transmitter.h>

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

    
    template <typename M>
    auto CreateTransmitter(const RoleAttributes& attr) ->
            typename std::shared_ptr<Transmitter<M>>;

    //返回在构造函数中创建的participant_
    ParticipantPtr participant() const { return participant_; }
private:
    void CreateParticipant();
    ParticipantPtr participant_ = nullptr;
    std::atomic<bool> is_shutdown_ = {false};

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

}
}
}

#endif
