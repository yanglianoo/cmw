#include <cmw/transport/shm/segment_factory.h>
#include <cmw/common/global_data.h>
#include <cmw/transport/shm/xsi_segment.h>
#include <cmw/common/log.h>
#include <cmw/transport/shm/posix_segment.h>
namespace hnu{
namespace cmw{
namespace transport{

using hnu::cmw::common::GlobalData;

auto SegmentFactory::CreateSegment(uint64_t channel_id) -> SegmentPtr{
    std::string segment_type(XsiSegment::Type());


    ADEBUG << "segment type: " << segment_type;
    if(segment_type == PosixSegment::Type()){
        return std::make_shared<PosixSegment>(channel_id);
    }
    return std::make_shared<XsiSegment>(channel_id);
}



}
}
}