#include <cmw/transport/shm/block.h>
#include <iostream>


namespace hnu{
namespace cmw{
namespace transport{

const int32_t Block::kRWLockFree = 0;
const int32_t Block::kWriteExclusive = -1; //代表正在写
const int32_t Block::kMaxTryLockTimes = 5;

Block::Block() : msg_size_(0) , msg_info_size_(0) {}

Block::~Block() {}

//对block加上写锁
bool Block::TryLockForWrite(){
    int32_t rw_lock_free = kRWLockFree;
    if(!lock_num_.compare_exchange_weak(rw_lock_free, kWriteExclusive,
                                       std::memory_order_acq_rel,
                                       std::memory_order_relaxed )){
            std::cout << "lock num: " << lock_num_.load() << std::endl;
    }
}

//加block加上读锁
bool Block::TryLockForRead() {
    int32_t lock_num  = lock_num_.load();
    if (lock_num < kRWLockFree) {
        //如果有其他线程正在写，则直接return，写的优先级比读高
        std::cout << "block is being written" << std::endl;
        return false;
    }
    int32_t try_times = 0;
    //如果此时没有线程持有锁，则将lock_num_的值加1
    while (!lock_num_.compare_exchange_weak(lock_num, lock_num + 1 ,
                                            std::memory_order_acq_rel,
                                            std::memory_order_relaxed))
    {
        //进入循环内部说明 lock_num_！= lock_num ，有另外一个线程拿到了锁
        ++try_times;
        //如果循环五次还没拿到读锁，则return
        if(try_times == kMaxTryLockTimes){
            std::cout << "fail to add read lock num, curr num: " << lock_num << std::endl;
            return false;
        }
 
        //再加载一次lock_num_
        lock_num = lock_num_.load();

        if(lock_num < kRWLockFree){
            //如果此时另外一个线程在写，直接跳出循环，不允许读
            std::cout << "block is being written" << std::endl;
            return false;
        }
    }

    return true;
    
}

//释放读锁
void Block::ReleaseReadLock() { lock_num_.fetch_add(1); }

//释放写锁
void Block::ReleaseWriteLock() { lock_num_.fetch_sub(1); }


}
}
}