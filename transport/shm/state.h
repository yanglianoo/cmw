#ifndef CYBER_TRANSPORT_SHM_STATE_H_
#define CYBER_TRANSPORT_SHM_STATE_H_


#include <atomic>
#include <cstring>
#include <mutex>

namespace hnu{
namespace cmw{
namespace transport{

class State
{

public:
    explicit State(const uint64_t& ceiling_msg_size);
    virtual ~State();

    
private:
    std::atomic<bool> need_remap_ = {false};
    std::atomic<uint32_t> seq_ = {0};
    std::atomic<uint32_t> reference_count_ = {0};
    std::atomic<uint64_t> ceiling_msg_size_;

};




}
}
}


#endif