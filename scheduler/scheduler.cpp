
#include <sched.h>
#include <utility>

#include <cmw/scheduler/scheduler.h>
#include <cmw/common/environment.h>
#include <cmw/common/file.h>
#include <cmw/common/global_data.h>
#include <cmw/common/util.h>
#include <cmw/data/data_visitor.h>
#include <cmw/scheduler/processor_context.h>

namespace hnu    {
namespace cmw   {
namespace scheduler {

using hnu::cmw::common::GlobalData;

bool Scheduler::CreateTask(const RoutineFactory& factory,
                           const std::string& name){
    return CreateTask(factory.create_routine(), name, factory.GetDataVisitor());
}

bool Scheduler::CreateTask(std::function<void()>&& func,
                           const std::string& name,
                           std::shared_ptr<DataVisitorBase> visitor){
    if(cyber_likely(stop_.load())){
        ADEBUG << "scheduler is stoped, cannot create task!";
        return false;
    }

    auto task_id = GlobalData::RegisterTaskName(name);

    auto cr = std::make_shared<CRoutine>(func);

    cr->set_id(task_id);
    cr->set_name(name);

    AINFO << "create croutine: " << name;

    if(!DispatchTask(cr)){
        return false;
    }

    if(visitor != nullptr){
        visitor->RegisterNotifyCallback([this, task_id](){
            if(cyber_likely(stop_.load())){
                return;
            }
            this->NotifyProcessor(task_id);
        });
    }

    return true;
}

bool Scheduler::NotifyTask(uint64_t crid){
    if(cyber_likely(stop_.load())){
        return true;
    }
    return NotifyProcessor(crid);
}

void Scheduler::ProcessLevelResourceControl() {
  std::vector<int> cpus;
  ParseCpuset(process_level_cpuset_, &cpus);
  cpu_set_t set;
  CPU_ZERO(&set);
  for (const auto cpu : cpus) {
    CPU_SET(cpu, &set);
  }
  pthread_setaffinity_np(pthread_self(), sizeof(set), &set);
}

}
}
}