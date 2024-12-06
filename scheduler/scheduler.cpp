
#include <sched.h>
#include <utility>

#include <cmw/scheduler/scheduler.h>
#include <cmw/common/environment.h>
#include <cmw/common/file.h>
#include <cmw/common/global_data.h>
#include <cmw/common/util.h>
#include <cmw/data/data_visitor.h>
#include <cmw/scheduler/processor_context.h>
#include <cmw/scheduler/processor.h>
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

    //新建一个croutine
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

void Scheduler::SetInnerThreadAttr(const std::string& name, std::thread* thr) {
  if (thr != nullptr && inner_thr_confs_.find(name) != inner_thr_confs_.end()) {
    auto th_conf = inner_thr_confs_[name];
    auto cpuset = th_conf.cpuset;

    std::vector<int> cpus;
    ParseCpuset(cpuset, &cpus);
    SetSchedAffinity(thr, cpus, "range");
    SetSchedPolicy(thr, th_conf.policy, th_conf.prio);
  }
}

void Scheduler::Shutdown() {
  if(cyber_likely(stop_.exchange(true))){
    return;
  }

  for(auto& ctx : pctxs_){
    ctx->Shutdown();
  }

  std::vector<uint64_t> cr_list;
  {
    ReadLockGuard<AtomicRWLock> lk(id_cr_lock_);
    for(auto& cr : id_cr_){
      cr_list.emplace_back(cr.second->id());
    }
  }

  for(auto& id : cr_list){
    RemoveCRoutine(id);
  }

  for(auto& processor : processors_){
    processor->Stop();
  }

  processors_.clear();
  pctxs_.clear();
}

void Scheduler::CheckSchedStatus(){
  std::string snap_info;
  auto now = Time::Now().ToNanosecond();
  for (auto processor : processors_) {
    auto snap = processor->ProcSnapshot();
    if (snap->execute_start_time.load()) {
      auto execute_time = (now - snap->execute_start_time.load()) / 1000000;
      snap_info.append(std::to_string(snap->processor_id.load()))
          .append(":")
          .append(snap->routine_name)
          .append(":")
          .append(std::to_string(execute_time));
    } else {
      snap_info.append(std::to_string(snap->processor_id.load()))
          .append(":idle");
    }
    snap_info.append(", ");
  }
  snap_info.append("timestamp: ").append(std::to_string(now));
  AINFO << snap_info;
  snap_info.clear();
}

}
}
}