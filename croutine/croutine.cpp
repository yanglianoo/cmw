
#include <algorithm>

#include <cmw/croutine/croutine.h>
#include <cmw/croutine/croutine_context.h>
#include <cmw/common/log.h>
#include <cmw/common/global_data.h>
#include <cmw/base/concurrent_object_pool.h>
#include <cmw/croutine/croutine_context.h>


namespace hnu {
namespace cmw {
namespace croutine{

thread_local CRoutine *CRoutine::current_routine_ = nullptr;
thread_local char *CRoutine::main_stack_ = nullptr;

namespace {
    std::shared_ptr<base::CCObjectPool<RoutineContext>> context_pool = nullptr;
    std::once_flag pool_init_flag;
    void CRoutineEntry(void *arg){
        CRoutine *r = static_cast<CRoutine *>(arg);
        r->Run();
        CRoutine::Yield(RoutineState::FINISHED);
    }
}

CRoutine::CRoutine(const std::function<void()> &func) : func_(func) {
    std::call_once(pool_init_flag, [&]{
        uint32_t routine_num = common::GlobalData::Instance()->ComponentNums();
        // 从cmw.pb.conf中读取 routine_num
        auto &global_conf = common::GlobalData::Instance()->Config();
        if(global_conf.scheduler_conf.routine_num){
            routine_num = 
                std::max(routine_num, global_conf.scheduler_conf.routine_num);
        }
        context_pool.reset(new base::CCObjectPool<RoutineContext>(routine_num));
    });

    context_ = context_pool->GetObject();
    if(context_ == nullptr){
        AWARN << "Maximum routine context number exceeded! Please check "
             "[routine_num] in config file.";
        context_.reset(new RoutineContext());
    }

    MakeContext(CRoutineEntry, this, context_.get());
    state_ = RoutineState::READY;
    updated_.test_and_set(std::memory_order_relaxed);
}

CRoutine::~CRoutine() { context_ = nullptr; }

RoutineState CRoutine::Resume() {
    if (cyber_unlikely(force_stop_)) {
        state_ = RoutineState::FINISHED;
        return state_;
    }

    if (cyber_unlikely(state_ != RoutineState::READY)) {
        AERROR << "Invalid Routine State!";
        return state_;
    }

    current_routine_ = this;
    SwapContext(GetMainStack(), GetStack());
    current_routine_ = nullptr;
    return state_;
}

void CRoutine::Stop() { force_stop_ = true; }

}
}
}