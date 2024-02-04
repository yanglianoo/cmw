#ifndef CMW_TRANSPORT_SHM_BLOCK_H_
#define CMW_TRANSPORT_SHM_BLOCK_H_


#include <atomic>
#include <cstdint>

namespace hnu{
namespace cmw{
namespace transport{


class Block
{

public:
    Block();
    virtual ~Block();

    uint64_t msg_size() const { return msg_size_; }
    void set_msg_size(uint64_t msg_size) { msg_size_ = msg_size;}
    uint64_t msg_info_size() const { return msg_info_size_; }
    void set_msg_info_size(uint64_t msg_info_size) {
        msg_info_size_ = msg_info_size;
    }

    static const int32_t kRWLockFree;
    static const int32_t kWriteExclusive;
    static const int32_t kMaxTryLockTimes;
    
private:
    bool TryLockForWrite();
    bool TryLockForRead();
    void ReleaseWriteLock();
    void ReleaseReadLock();

    std::atomic<int32_t> lock_num_ = {0};
    uint64_t msg_size_;
    uint64_t msg_info_size_;
};



}
}
}

#endif