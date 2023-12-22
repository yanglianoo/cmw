
#include <cmw/discovery/communication/reader_listener.h>

namespace hnu {
namespace cmw {
namespace discovery{ 

ReaderListener::ReaderListener(const NewMsgCallback& callback)
    : callback_(callback) {}

ReaderListener::~ReaderListener(){
    std::lock_guard<std::mutex> lck(mutex_);
    callback_ = nullptr;
}

void ReaderListener::onNewCacheChangeAdded(
    eprosima::fastrtps::rtps::RTPSReader* reader,
    const eprosima::fastrtps::rtps::CacheChange_t* const change){
    
    if(callback_ == nullptr){
        std::cout << "callback_ is nullptr" << std::endl;
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_); 
    std::shared_ptr<std::string> msg_str = 
        std::make_shared<std::string>((char*)change->serializedPayload.data,change->serializedPayload.length);
    callback_(*msg_str);
}




}
}
}