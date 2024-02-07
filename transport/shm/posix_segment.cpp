

#include <cmw/transport/shm/posix_segment.h>
#include <cmw/transport/shm/shm_conf.h>
#include <cmw/transport/shm/block.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <iostream>
#include <unistd.h>
namespace hnu{
namespace cmw{
namespace transport{

PosixSegment::PosixSegment(uint64_t channel_id) : Segment(channel_id) {
    shm_name_ = std::to_string(channel_id);
}

PosixSegment::~PosixSegment() { Destroy(); }

//创建共享内存块
bool PosixSegment::OpenOrCreate() {
    if(init_){
        return true;
    }
    
    int fd = shm_open(shm_name_.c_str() , O_RDWR | O_CREAT | O_EXCL , 0644);  //0644代表只有共享内存的所有者才有写权限
    if(fd < 0){
        if(EEXIST == errno){
            std::cout << "shm already exist, open only."<< std::endl;
            return OpenOnly();
        } else {
            std::cout << "create shm failed, error: " << strerror(errno) << std::endl;
            return false;
        }
    }

    //将共享内存的小更改成 conf_.managed_shm_size()
    if(ftruncate(fd , conf_.managed_shm_size()) < 0){
        std::cout << "ftruncate failed: " << strerror(errno);
        close(fd);
        return false;
    }

      // 映射共享内存，可读可写 ，
    managed_shm_ = mmap(nullptr, conf_.managed_shm_size(), PROT_READ | PROT_WRITE,
                      MAP_SHARED, fd, 0);

    //如果映射失败
    if(managed_shm_ == MAP_FAILED){
        std::cout << "attach shm failed:" << strerror(errno);
        close(fd);
        shm_unlink(shm_name_.c_str());  //删除共享内存对象
        return false;
    }

    close(fd);

    state_ = new (managed_shm_) State(conf_.ceiling_msg_size());  //在共享内存起始地址创建一个State对象
    if( state_ == nullptr ){
        std::cout << "create state failed.";
        munmap(managed_shm_, conf_.managed_shm_size());  //解除映射
        managed_shm_ = nullptr;
        shm_unlink(shm_name_.c_str());
        return false;
    }

    //创建blocks
    blocks_ = new (static_cast<char*>(managed_shm_) + sizeof(State)) 
        Block[conf_.block_num()];
    if(blocks_ == nullptr){
        std::cout << "create blocks failed." << std::endl;
        state_->~State();
        state_ = nullptr;
        munmap(managed_shm_, conf_.managed_shm_size());
        managed_shm_ = nullptr;
        shm_unlink(shm_name_.c_str());
        return false;
    }

    //创建 block buf
    uint32_t i = 0;
    for (; i < conf_.block_num(); ++i)
    {
        uint8_t* addr = 
            new (static_cast<char*>(managed_shm_) + sizeof(State) + 
                 conf_.block_num() * sizeof(Block) + i * conf_.block_buf_size())
                 uint8_t[conf_.block_buf_size()];
        if(addr == nullptr){
            break;
        }

        std::lock_guard<std::mutex> lg(block_buf_lock_);
        block_buf_addrs_[i] = addr;
    }

    if ( i != conf_.block_num()){
        std::cout<< "create block buf failed.";
        state_->~State();
        state_ = nullptr;
        blocks_ = nullptr;
        {
        std::lock_guard<std::mutex> lg(block_buf_lock_);
        block_buf_addrs_.clear();
        }
        munmap(managed_shm_, conf_.managed_shm_size());
        managed_shm_ = nullptr;
        shm_unlink(shm_name_.c_str());
        return false;
    }

    state_->IncreaseReferenceCounts();
    init_ = true;
    return true;
}


bool PosixSegment::OpenOnly(){
    if(init_){
        return true;
    }

    //打开一个已存在的共享内存对象
    int fd = shm_open(shm_name_.c_str() , O_RDWR , 0644);

    if(fd == -1){
        std::cout << "get shm failed: " << strerror(errno)<< std::endl;
        return false;
    }
    //fstat函数用于获取打开的文件的属性
    struct stat file_attr;
    if (fstat(fd, &file_attr) < 0) {
        std::cout << "fstat failed: " << strerror(errno)<< std::endl;
        close(fd);
        return false;
    }

    //映射共享内存
    managed_shm_ = mmap(nullptr, file_attr.st_size, PROT_READ | PROT_WRITE,
                      MAP_SHARED, fd, 0);

    //如果映射失败
    if(managed_shm_ == MAP_FAILED){
        std::cout << "attach shm failed:" << strerror(errno);
        close(fd);
        return false;
    }

    close(fd);  
    //直接转换
    state_ = reinterpret_cast<State*>(managed_shm_);         
    if (state_ == nullptr) {
        std::cout << "get state failed." << std::endl;
        munmap(managed_shm_, file_attr.st_size);
        managed_shm_ = nullptr;
        return false;
    }

    conf_.Update(state_->ceiling_msg_size());

    blocks_ = reinterpret_cast<Block*>(static_cast<char*>(managed_shm_) + sizeof(State));

    if(blocks_ == nullptr){
        std::cout << "get blocks failed." << std::endl;
        state_ = nullptr;
        munmap(managed_shm_, conf_.managed_shm_size());
        managed_shm_ = nullptr;
        return false;
    }

    uint32_t i = 0;
    //拿到block 的 buf的地址
    for (; i < conf_.block_num(); ++i){
        uint8_t* addr = reinterpret_cast<uint8_t*>(
                static_cast<char*>(managed_shm_) + sizeof(State) + 
                conf_.block_num() * sizeof(Block) + i * conf_.block_buf_size());
    }

    if (i != conf_.block_num()) {
        std::cout << "open only failed." << std::endl;
        state_->~State();
        state_ = nullptr;
        blocks_ = nullptr;
        {
        std::lock_guard<std::mutex> lg(block_buf_lock_);
        block_buf_addrs_.clear();
        }
        munmap(managed_shm_, conf_.managed_shm_size());
        managed_shm_ = nullptr;
        shm_unlink(shm_name_.c_str());
        return false;
   }

    state_->IncreaseReferenceCounts();
    init_ = true;
    std::cout  << "open only true." << std::endl;
    return true; 
}

//删除共享内存
bool PosixSegment::Remove() {
  if (shm_unlink(shm_name_.c_str()) < 0) {
    std::cout << "shm_unlink failed: " << strerror(errno) << std::endl;
    return false;
  }
  return true;
}

//重置共享内存,解除映射
void PosixSegment::Reset() {
  state_ = nullptr;
  blocks_ = nullptr;
  {
    std::lock_guard<std::mutex> lg(block_buf_lock_);
    block_buf_addrs_.clear();  // 清空buf 管理map
  }
  if (managed_shm_ != nullptr) {
    munmap(managed_shm_, conf_.managed_shm_size()); //解除共享内存映射
    managed_shm_ = nullptr;
    return;
  }
}


}
}
}