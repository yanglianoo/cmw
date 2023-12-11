#ifndef CMW_TRANSPORT_TRANSPORT_H_
#define CMW_TRANSPORT_TRANSPORT_H_


#include <cmw/transport/rtps/participant.h>
#include <cmw/common/macros.h>
#include <cmw/config/RoleAttributes.h>
#include <cmw/transport/transmitter/transmitter.h>
namespace hnu    {
namespace cmw   {
namespace transport {

using namespace config;
class Transport
{
public:
    virtual ~Transport();

    void Shutdown();


    template <typename M>
    auto CreateTransmitter(const RoleAttributes& attr) ->
            typename std::shared_ptr<Transmitter<M>>;


    ParticipantPtr participant() const { return participant_; }
private:
    void CreateParticipant();
    ParticipantPtr participant_ = nullptr;
    std::atomic<bool> is_shutdown_ = {false};

    DECLARE_SINGLETON(Transport)
};

}
}
}

#endif
