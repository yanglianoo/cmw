#ifndef CMW_DISCOVERY_COMMUNICATION_PARTICIPANT_LISTENER_H_
#define CMW_DISCOVERY_COMMUNICATION_PARTICIPANT_LISTENER_H_


#include <fastrtps/rtps/participant/RTPSParticipantListener.h>
#include <mutex>
#include <functional>

namespace hnu {
namespace cmw {
namespace discovery{ 


class ParticipantListener: public eprosima::fastrtps::rtps::RTPSParticipantListener{

public:
    using ChangeFunc = std::function<void(
        const eprosima::fastrtps::rtps::ParticipantDiscoveryInfo& info)>;
    
    explicit ParticipantListener(const ChangeFunc& callback);
    virtual ~ParticipantListener();

    virtual void onParticipantDiscovery(
            eprosima::fastrtps::rtps::RTPSParticipant* p,
            eprosima::fastrtps::rtps::ParticipantDiscoveryInfo info);
private:
    ChangeFunc callback_;
    std::mutex mutex_;

};






}
}
}

#endif