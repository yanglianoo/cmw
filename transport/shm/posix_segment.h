

#ifndef CMW_TRANSPORT_SHM_POSIX_SEGMENT_H_
#define CMW_TRANSPORT_SHM_POSIX_SEGMENT_H_


#include <string>
#include <cmw/transport/shm/segment.h>

namespace hnu{
namespace cmw{
namespace transport{

class PosixSegment : public Segment{
    public:
        explicit PosixSegment(uint64_t channel_id);
        virtual ~PosixSegment();

        static const char* Type() { return "posix"; }
        
    private:
        void Reset() override;
        bool Remove() override;
        bool OpenOnly() override;
        bool OpenOrCreate() override;

        std::string shm_name_;

};

}
}
}

#endif