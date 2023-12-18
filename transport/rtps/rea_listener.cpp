#include <cmw/transport/rtps/rea_listener.h>
#include <fastrtps/rtps/reader/RTPSReader.h>
#include <fastrtps/rtps/rtps_fwd.h>
#include <fastrtps/rtps/common/SequenceNumber.h>
#include <memory.h>

using GUID_t = eprosima::fastrtps::rtps::GUID_t;

namespace hnu    {
namespace cmw   {
namespace transport {

ReaListener::ReaListener( const NewMsgCallback& callback)
         : callback_(callback) {}

ReaListener::~ReaListener() {}


void ReaListener::onNewCacheChangeAdded(
                eprosima::fastrtps::rtps::RTPSReader* reader,
                const eprosima::fastrtps::rtps::CacheChange_t* const change) 
{

    std::lock_guard<std::mutex> lock(mutex_);

    //GUID_t是rtps中实体的标识符,每个实体都是唯一的
    char* ptr = reinterpret_cast<char*>(const_cast<GUID_t*>
                            (&change->write_params.sample_identity().writer_guid()));

    Identity sender_id(false);
    sender_id.set_data(ptr);
    msg_info_.set_sender_id(sender_id);

    Identity spare_id(false);
    spare_id.set_data(ptr + ID_SIZE);
    msg_info_.set_spare_id(spare_id);

    uint64_t seq_num = ((int64_t)change->sequenceNumber.high << 32) | change->sequenceNumber.low;

    msg_info_.set_seq_num(seq_num);

    // 这里可以修改callback 直接传递二进制数据流的数据和长度 
    std::shared_ptr<std::string> msg_str = 
        std::make_shared<std::string>((char*)change->serializedPayload.data,change->serializedPayload.length);

    callback_(msg_str, msg_info_);
}
}
}
}