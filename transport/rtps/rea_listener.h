#ifndef CMW_TRANSPORT_RTPS_REA_LISTENER_H_
#define CMW_TRANSPORT_RTPS_REA_LISTENER_H_

#include <fastrtps/rtps/rtps_fwd.h>
#include <fastrtps/rtps/reader/ReaderListener.h>
#include <cmw/transport/message/message_info.h>
#include <memory>
#include <mutex>
#include <functional>
namespace hnu    {
namespace cmw   {
namespace transport {


class ReaListener;
using RealistenerPtr = std::shared_ptr<ReaListener>;

//eprosima::fastrtps::rtps::ReaderListener 是一个异步的线程用于监控 Cache中是否有数据
class ReaListener : public eprosima::fastrtps::rtps::ReaderListener
{
public:
        using NewMsgCallback = std::function<void(
            uint64_t channel_id,
            const std::shared_ptr<std::string>& msg_str, 
            const MessageInfo& msg_info)>;

        explicit ReaListener(const NewMsgCallback& callback, const std::string& channel_name);
        virtual ~ReaListener();
        
        void onNewCacheChangeAdded(
                eprosima::fastrtps::rtps::RTPSReader* reader,
                const eprosima::fastrtps::rtps::CacheChange_t* const change) override;
        void onReaderMatched(
                eprosima::fastrtps::rtps::RTPSReader*,
                eprosima::fastrtps::rtps::MatchingInfo& info) override
        {
            if (info.status == eprosima::fastrtps::rtps::MATCHED_MATCHING)
            {
                printf("matched\n");
                n_matched++;
            }
        }
private:
    uint32_t n_matched;
    std::string channel_name_;
    // fast-rtps reader的listener的回调函数，会在onNewCacheChangeAdded 中调用
    NewMsgCallback callback_;
    MessageInfo msg_info_;
    std::mutex mutex_;
};




}
}
}


#endif