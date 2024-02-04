
#include <cmw/transport/shm/segment.h>
#include <cmw/base/macros.h>
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
    writable_block->index = index;
    writable_block->block = &blocks_[index];
    writable_block->buf = block_buf_addrs_[index];
    return true;
}


}
}
}