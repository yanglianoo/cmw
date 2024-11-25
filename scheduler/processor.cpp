#include <cmw/scheduler/processor.h>

#include <sched.h>
#include <sys/resource.h>
#include <sys/syscall.h>

#include <chrono>

#include <cmw/common/global_data.h>
#include <cmw/common/log.h>
#include <cmw/croutine/croutine.h>
#include <cmw/time/time.h>

namespace hnu    {
namespace cmw   {
namespace scheduler {

using hnu::cmw::common::GlobalData;


Processor::Processor() { running_.store(true); }

Processor::~Processor() { Stop(); }

void Processor::Run(){
    tid_.store(static_cast<int>(syscall(SYS_gettid)));

    AINFO << "processor_tid: " << tid_;

    snap_shot_->processor_id.store(tid_);

    while (cyber_likely(running_.load()))
    {
        if(cyber_likely(context_ != nullptr)){
            auto croutine = context_->NextRoutine();
            if(croutine){
                snap_shot_->execute_start_time.store(hnu::cmw::Time::Now().ToNanosecond());
                snap_shot_->routine_name = croutine->name();
                croutine->Resume();
                croutine->Release();
            }else {
                snap_shot_->execute_start_time.store(0);
                context_->Wait();
            }
        }else{
            std::unique_lock<std::mutex> lk(mtx_ctx_);
            cv_ctx_.wait_for(lk, std::chrono::milliseconds(10));
        }
    }
    
}

void Processor::Stop(){
    if(!running_.exchange(false)){
        return;
    }

    if(context_){
        context_->Shutdown();
    }

    cv_ctx_.notify_one();
    if(thread_.joinable()){
        thread_.join();
    }
}

void Processor::BindContext(const std::shared_ptr<ProcessorContext>& context){
    context_ = context;
    std::call_once(thread_flag_,
                   [this](){ thread_ = std::thread(&Processor::Run, this); });
}

std::atomic<pid_t>& Processor::Tid() {
  while (tid_.load() == -1) {
    cpu_relax();
  }
  return tid_;
}


}
}
}