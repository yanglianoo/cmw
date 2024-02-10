#ifndef CYBER_TRANSPORT_SHM_XSI_SEGMENT_H_
#define CYBER_TRANSPORT_SHM_XSI_SEGMENT_H_

#include <cmw/transport/shm/segment.h>


namespace hnu{
namespace cmw{
namespace transport{

class XsiSegment : public Segment{

public:
    explicit XsiSegment(uint64_t channel_id);
    virtual ~XsiSegment();

    static const char* Type() { return "xsi"; }

private:
    void Reset() override;
    bool Remove() override;
    bool OpenOnly() override;
    bool OpenOrCreate() override;

    key_t key_;

};

}
}
}

#endif