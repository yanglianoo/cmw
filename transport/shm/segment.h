#ifndef CMW_TRANSPORT_SHM_SEGMENT_H_
#define CMW_TRANSPORT_SHM_SEGMENT_H_


#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include <cmw/transport/shm/block.h>
#include <cmw/transport/shm/shm_conf.h>
#include <cmw/transport/shm/state.h>


namespace hnu{
namespace cmw{
namespace transport{



struct WritableBlock
{
    uint32_t index = 0;
    Block* block = nullptr;
    uint8_t* buf = nullptr;
};

using ReadableBlock = WritableBlock;

class Segment
{

public:
    explicit Segment(uint64_t channel_id);
    virtual ~Segment() {}

    bool AcquireBlockToWrite(std::size_t msg_size, WritableBlock* writable_block);
    void ReleaseWrittenBlock(const WritableBlock& writable_block);

    bool AcquireBlockToRead(ReadableBlock* readable_block);
    void ReleaseReadBlock(const ReadableBlock& readable_block);



protected:
    virtual bool Destroy();
    virtual void Reset() = 0;
    virtual bool Remove() = 0;
    virtual bool OpenOnly() = 0;
    virtual bool OpenOrCreate() = 0;
    bool init_;
    ShmConf conf_;
    uint64_t channel_id_;

    State* state_;
    Block* blocks_;
    void* managed_shm_;
    std::mutex block_buf_lock_;
    std::unordered_map<uint32_t, uint8_t*> block_buf_addrs_;

private:
    bool Remap();
    bool Recreate(const uint64_t& msg_size);
    uint32_t GetNextWritableBlockIndex();
};






}
}
}
#endif