#ifndef CMW_TRANSPORT_TRANSPORT_H_
#define CMW_TRANSPORT_TRANSPORT_H_

#include <fastrtps/rtps/participant/RTPSParticipant.h>
#include <fastrtps/rtps/RTPSDomain.h>
namespace hnu    {
namespace cmw   {
namespace transport {

class Transport
{
public:

private:
    void CreateParticipant();
    eprosima::fastrtps::rtps::RTPSParticipant* mp_participant;
};

}
}
}

#endif
