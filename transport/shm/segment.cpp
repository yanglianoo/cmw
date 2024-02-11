
#include <cmw/transport/shm/segment.h>
#include <cmw/common/log.h>
#include <iostream>
namespace hnu{
namespace cmw{
namespace transport{

Segment::Segment(uint64_t channel_id) 
    : init_(false),
      conf_(),
      channel_id_(channel_id),
      state_(nullptr),
      blocks_(nullptr),
      managed_shm_(nullptr),
      block_buf_lock_(),
      block_buf_addrs_() {}

bool Segment::AcquireBlockToWrite(std::size_t msg_size, WritableBlock* writable_block){
    RETURN_VAL_IF_NULL(writable_block ,false);
    if(!init_ && !OpenOrCreate()){
        std::cout << "create shm failed, can't write now." << std::endl;
        return false;
    }

    bool result = true;
    if(state_->need_remap()){
        result = Remap();
    }

    //如果msg_size 超过默认的 size，则销毁之前创建的共享内存，重新创建一块更大的内存
    if(msg_size > conf_.ceiling_msg_size()){
       std::cout<< "msg_size: " << msg_size
                << " larger than current shm_buffer_size: "
                << conf_.ceiling_msg_size() << " , need recreate." << std::endl;
                result = Recreate(msg_size);
    }

    if(!result){
        std::cout << "segment update failed." << std::endl;
        return false;
    }

    uint32_t index = GetNextWritableBlockIndex();
    //将writable_block 指向 blocks_[index]
    writable_block->index = index;
    writable_block->block = &blocks_[index];
    writable_block->buf = block_buf_addrs_[index];
    return true;
}


void Segment::ReleaseWrittenBlock(const WritableBlock& writable_block){
    auto index = writable_block.index;
    if( index >= conf_.block_num()){
        return;
    }
    //释放写锁
    blocks_[index].ReleaseWriteLock();
}

bool Segment::AcquireBlockToRead(ReadableBlock* readable_block){
    RETURN_VAL_IF_NULL(readable_block, false);
    if(!init_ && !OpenOnly()){
        AERROR << "failed to open shared memory, can't read now." ;
        return false;       
    }

    auto index = readable_block->index;
    if(index >= conf_.block_num()){
        AERROR << "invalid block_index[" << index << "].";
        return false;
    }

    bool result = true;
    if( state_->need_remap() ){
        result = Remap();
    }

    if(!result){
        AERROR << "segment update failed." ;
        return false;
    }

    if(!blocks_[index].TryLockForRead()){
        return false;
    }
    readable_block->block = blocks_ + index;
    readable_block->buf = block_buf_addrs_[index];
    return true;
}

//释放Block的读锁
void Segment::ReleaseReadBlock(const ReadableBlock& readable_block){
    auto index = readable_block.index;
    if(index >= conf_.block_num()){
        return;
    }
    blocks_[index].ReleaseReadLock();
}

bool Segment::Destroy(){
    if(!init_){
        return true;
    }
    init_ = false;

    try
    {
        state_->DecreaseReferenceCounts();
        uint32_t reference_counts = state_->reference_counts();
        if(reference_counts == 0){
            return Remove();
        }
    }
    catch(...)
    {
        std::cout << "exception." << std::endl;
        return false;
    }

    std::cout << "destroy." << std::endl;
    return true;
    
}


bool Segment::Remap(){
    init_ = false;
    std::cout << "before reset." << "\n";
    Reset();
    std::cout << "after reset." << "\n";
    return OpenOnly();
}

bool Segment::Recreate(const uint64_t& msg_size){
    init_ = false;
    state_->set_need_remap(true);
    Reset();
    Remove();
    conf_.Update(msg_size);
    return OpenOrCreate();
}

uint32_t Segment::GetNextWritableBlockIndex(){
    const auto block_num = conf_.block_num();
    while (1)
    {
        //返回seq，并将seq加一
        uint32_t try_idx = state_->FetchAddSeq(1) % block_num;
        //为blocks_[try_idx] 这块内存加上写锁
        if(blocks_[try_idx].TryLockForWrite()){
            return try_idx;
        }
    }
    return 0;
}

}
}
}