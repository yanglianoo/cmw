#include <cmw/transport/shm/condition_notifier.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <thread>
#include <cmw/common/util.h>
#include <cmw/common/log.h>
namespace hnu{
namespace cmw{
namespace transport{

using common::Hash;

ConditionNotifier::ConditionNotifier(){
    key_ = static_cast<key_t>(Hash("/hnu/cmw/transport/shm/notifier"));
    shm_size_ = sizeof(Indicator);

    if(!Init()){
        AERROR << "fail to init condition notifier." ;
        is_shutdown_.store(true);
        return;
    }

    next_seq_ = indicator_->next_seq.load();
    ADEBUG << "next_seq: " << next_seq_;
}

ConditionNotifier::~ConditionNotifier() {Shutdown();}

void ConditionNotifier::Shutdown( ){
    if(is_shutdown_.exchange(true)){
        return;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    Reset();
}


bool ConditionNotifier::Notify(const ReadableInfo& info){
    if(is_shutdown_.load()){
        ADEBUG << "notifier is shutdown.";
        return false;
    }
    //先取到next_seq，再对next_seq+1
    uint64_t seq = indicator_->next_seq.fetch_add(1);

    //填充要通知的信息
    uint64_t idx = seq % kBufLength;
    indicator_->infos[idx] = info;
    indicator_->seqs[idx] = seq;

    return true;
}

bool ConditionNotifier::Listen(int timeout_ms ,ReadableInfo* info){
    if(info == nullptr){
        AERROR << "info nullptr" ;
        return false;
    }

    if(is_shutdown_.load()){
        ADEBUG << "notifier is shutdown." ;
    }

    int timeout_us = timeout_ms * 1000;
    while (!is_shutdown_.load())
    {   
        
        uint64_t seq = indicator_->next_seq.load();

        //如果有其他进程 执行了Notify，则 seq != next_seq_ ,说明有新的info
        if(seq != next_seq_){
            auto idx = next_seq_ % kBufLength;
            auto actual_seq = indicator_->seqs[idx];
            //
            if(actual_seq >= next_seq_){
                next_seq_ = actual_seq;
                *info = indicator_->infos[idx];
                ++next_seq_;
                return true;
            } else {
                ADEBUG << "seq[" << next_seq_ << "] is writing, can not read now.";
            }
        }

        if(timeout_us > 0){
            std::this_thread::sleep_for(std::chrono::microseconds(50));
            timeout_us -= 50;
        } else {
            return false;
        }
    }

    return false;
    
}


bool ConditionNotifier::Init() { return OpenOrCreate(); }


bool ConditionNotifier::OpenOrCreate(){
    int retry = 0;
    int shmid = 0;
    while (retry < 2)
    {
        shmid = shmget(key_, shm_size_, 0644 | IPC_CREAT | IPC_EXCL);
        if(shmid != -1){
            break;
        }

        if(EINVAL == errno){
            AINFO << "need larger space, recreate.";
            Reset();
            Remove();
            ++retry;
        } else if( EEXIST == errno){
            ADEBUG << "shm already exist, open only.";
            return OpenOnly();
        } else {
            break;
        }
    }

    if(shmid == -1){
            AERROR << "create shm failed, error code: " << strerror(errno);
            return false;
    }

    managed_shm_ = shmat(shmid, nullptr ,0);

    if(managed_shm_ == reinterpret_cast<void*>(-1)){
        AERROR << "attach shm failed.";
        shmctl(shmid, IPC_RMID, 0);
        return false;
    }

    indicator_ = new (managed_shm_) Indicator();

    if(indicator_ == nullptr){
        AERROR << "create indicator failed." ;
        shmdt(managed_shm_);
        managed_shm_ = nullptr;
        shmctl(shmid, IPC_RMID, 0);
        return false;
    }

    ADEBUG  << "open or create true.";
    return true;
    
}

bool ConditionNotifier::OpenOnly(){
    
    int shmid = shmget(key_, 0 , 0644);
    if(shmid == -1){
        AERROR << "get shm failed, error: " << strerror(errno);
        return false; 
    }

    //映射共享内存
    managed_shm_ = shmat(shmid, nullptr , 0);
    if(managed_shm_ == reinterpret_cast<void*>(-1)){
        AERROR << "attach shm failed.";
        shmctl(shmid, IPC_RMID, 0);
        return false;
    }

    indicator_ = reinterpret_cast<Indicator*>(managed_shm_);
    if(indicator_ == nullptr){
        AERROR << "get indicator failed." ;
        shmdt(managed_shm_);
        managed_shm_ = nullptr;
        return false;
    }

    ADEBUG << "Open true" ;

    return true;


}


//删除共享内存
bool ConditionNotifier::Remove(){
    int shmid = shmget(key_, 0 , 0644);
    if(shmid == -1 || shmctl(shmid, IPC_RMID,0) == -1){
        AERROR<< "remove shm failed, error code: " << strerror(errno) ;
        return false;
    }

    ADEBUG<< "remove success.";
    return true;
}

//shm detach 断开与共享内存的连接
void ConditionNotifier::Reset(){
    indicator_ = nullptr;
    if(managed_shm_ != nullptr){
        shmdt(managed_shm_);
        managed_shm_ = nullptr;
    }
}


}
}
}