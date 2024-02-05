#ifndef CMW_TRANSPORT_SHM_SEGMENT_FACTORY_H_
#define CMW_TRANSPORT_SHM_SEGMENT_FACTORY_H_


#include <cmw/transport/shm/segment.h>


namespace hnu{
namespace cmw{
namespace transport{


class SegmentFactory{
    public:
        static SegmentPtr CreateSegment(uint64_t channel_id);
};

}
}
}

#endif