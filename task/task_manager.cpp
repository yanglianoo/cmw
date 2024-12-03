#include <cmw/task/task_manager.h>
#include <cmw/croutine/croutine.h>
#include <cmw/croutine/croutine_factory.h>
#include <cmw/scheduler/scheduler_factory.h>


namespace hnu {
namespace cmw {

using hnu::cmw::common::GlobalData;
static const char* const task_prefix = "/internal/task";

TaskManager::TaskManager() : task_queue_size_(1000),
                             task_queue_(new base::BoundedQueue<std::function<void()>>()){
    if (!task_queue_->Init(task_queue_size_, new base::BlockWaitStrategy())) {
        AERROR << "Task queue init failed";
        throw std::runtime_error("Task queue init failed");
    }

    auto func = [this]() {
        while (!stop_) {
            std::function<void()> task;
            if (!task_queue_->Dequeue(&task)) {
                auto routine = croutine::CRoutine::GetCurrentRoutine();
                routine->HangUp();
                continue;
            }
            task();
        }
    };
    
    num_threads_ = scheduler::Instance()->TaskPoolSize();
    auto factory = croutine::CreateRoutineFactory(std::move(func));
    tasks_.reserve(num_threads_);
    for (uint32_t i = 0; i < num_threads_; i++) {
        auto task_name = task_prefix + std::to_string(i);
        tasks_.push_back(common::GlobalData::RegisterTaskName(task_name));
        if (!scheduler::Instance()->CreateTask(factory, task_name)) {
            AERROR << "CreateTask failed:" << task_name;
        }
    }
}
TaskManager::~TaskManager() { Shutdown(); }

void TaskManager::Shutdown() {
  if (stop_.exchange(true)) {
    return;
  }
  for (uint32_t i = 0; i < num_threads_; i++) {
    scheduler::Instance()->RemoveTask(task_prefix + std::to_string(i));
  }
}

}
}