#ifndef CMW_DISCOVERY_COMMUNICATION_READER_LISTENER_H_
#define CMW_DISCOVERY_COMMUNICATION_READER_LISTENER_H_

#include <fastrtps/rtps/rtps_fwd.h>
#include <fastrtps/rtps/reader/ReaderListener.h>
#include <cmw/transport/message/message_info.h>
#include <memory>
#include <mutex>
#include <functional>
namespace hnu {
namespace cmw {
namespace discovery{ 


class ReaderListener : public eprosima::fastrtps::rtps::ReaderListener
{
public:
    using NewMsgCallback  = std::function<void(const std::string&)>;
    explicit ReaderListener(const NewMsgCallback& callback);
    virtual ~ReaderListener();

    void onNewCacheChangeAdded(
            eprosima::fastrtps::rtps::RTPSReader* reader,
            const eprosima::fastrtps::rtps::CacheChange_t* const change) override;
    
    void onReaderMatched(
                eprosima::fastrtps::rtps::RTPSReader*,
                eprosima::fastrtps::rtps::MatchingInfo& info) override{
        }
private:
    NewMsgCallback callback_;
    transport::MessageInfo msg_info_;
    std::mutex mutex_;
};



}
}
}

#endif
