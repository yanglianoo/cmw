#include <cmw/discovery/communication/participant_listener.h>
#include <cmw/base/macros.h>


namespace hnu {
namespace cmw {
namespace discovery{ 


ParticipantListener::ParticipantListener(const ChangeFunc& callback) 
        : callback_(callback) {}

ParticipantListener::~ParticipantListener(){
    std::lock_guard<std::mutex> lck(mutex_);
    callback_ = nullptr;
}

void ParticipantListener::onParticipantDiscovery(
            eprosima::fastrtps::rtps::RTPSParticipant* p,
            eprosima::fastrtps::rtps::ParticipantDiscoveryInfo info){
    
    RETURN_IF_NULL(callback_);
    (void)p;
    std::lock_guard<std::mutex> lock(mutex_);
    callback_(info);
}


}
}
}