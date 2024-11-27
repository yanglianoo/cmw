#include <cmw/common/environment.h>
#include <cmw/common/file.h>
#include <cmw/scheduler/policy/classic_context.h>
#include <cmw/scheduler/processor.h>
#include <cmw/scheduler/policy/scheduler_classic.h>
#include <cmw/config/cmw_conf.h>
#include <cmw/config/conf_parse.h>

namespace hnu    {
namespace cmw   {
namespace scheduler {

using hnu::cmw::base::ReadLockGuard;
using hnu::cmw::base::WriteLockGuard;
using hnu::cmw::common::GetAbsolutePath;
using hnu::cmw::config::GetCmwConfFromFile;
using hnu::cmw::common::GlobalData;
using hnu::cmw::common::PathExists;
using hnu::cmw::common::WorkRoot;
using hnu::cmw::croutine::RoutineState;


SchedulerClassic::SchedulerClassic(){

    std::string conf("conf/");
    conf.append(GlobalData::Instance()->ProcessGroup()).append(".conf");

    // cfg_file = CMW_PATH/conf/***.conf 
    auto cfg_file = GetAbsolutePath(WorkRoot(), conf);

    hnu::cmw::config::CmwConfig cfg;

    if(PathExists(cfg_file) && GetCmwConfFromFile(cfg_file, &cfg)){
        for(auto& thr : cfg.scheduler_conf.threads){
            inner_thr_confs_[thr.name] = thr;
        }
        
        if(!cfg.scheduler_conf.process_level_cpuset.empty()){
            process_level_cpuset_ = cfg.scheduler_conf.process_level_cpuset;
            AINFO << "scheduler_conf.process_level_cpuset: " << cfg.scheduler_conf.process_level_cpuset;
        }

        classic_conf_ = cfg.scheduler_conf.classic_conf;
        for(auto& group : classic_conf_.groups){
            auto group_name = group.name;
            for(auto task : group.tasks){
                task.group_name = group_name;
                cr_confs_[task.name] = task;
                AINFO << "cr_confs_[" << task.name <<"]";
            }
        }
    } else {
        // if do not set default_proc_num in scheduler conf
        // give a default value
        uint32_t proc_num = 2;
        auto& global_conf = GlobalData::Instance()->Config();
        if(global_conf.scheduler_conf.default_proc_num){
            proc_num = global_conf.scheduler_conf.default_proc_num;
        }

        task_pool_size_ = proc_num;

        classic_conf_.groups.emplace_back();

        auto sched_group = classic_conf_.groups.back();

        sched_group.name = DEFAULT_GROUP_NAME;
        sched_group.processor_num = proc_num;
    }

    CreateProcessor();
}
/**
scheduler_conf {
  policy: "classic"
  classic_conf {
    groups: [
      {
        name: "control"
        processor_num: 8
        affinity: "range"
        cpuset: "8-15"
        processor_policy: "SCHED_OTHER"
        processor_prio: 0
        tasks: [
          {
            name: "control_/apollo/planning"
            prio: 10
          },
          {
            name: "canbus_/apollo/control"
            prio: 11
          }
        ]
      }
    ]
  }
}
 */
void SchedulerClassic::CreateProcessor(){
    for(auto& group : classic_conf_.groups){
        auto& group_name = group.name;
        AINFO << "group_name: " << group_name ;
        auto proc_num = group.processor_num;
        if(task_pool_size_ == 0){
            task_pool_size_ = proc_num;
        }

        auto& affinity = group.affinity;
        auto& processor_policy = group.processor_policy;
        auto processor_prio = group.processor_prio;
        AINFO << "affinity: " << affinity ;
        AINFO << "processor_policy: " << processor_policy ;
        AINFO << "processor_prio: " << processor_prio ;
        AINFO << "cpuset: " << group.cpuset ;
        std::vector<int> cpuset;

        ParseCpuset(group.cpuset, &cpuset);
        // 根据proc_num数量创建Processor
        for(uint32_t i=0; i<proc_num; i++){
            auto ctx = std::make_shared<ClassicContext>(group_name);
            pctxs_.emplace_back(ctx);

            auto proc = std::make_shared<Processor>();
            proc->BindContext(ctx);

            SetSchedAffinity(proc->Thread(), cpuset, affinity, i);
            SetSchedPolicy(proc->Thread(), processor_policy, processor_prio,
                     proc->Tid());
            processors_.emplace_back(proc);
        }
    }
}

bool SchedulerClassic::DispatchTask(const std::shared_ptr<CRoutine>& cr){

    MutexWrapper* wrapper = nullptr;
    if(!id_map_mutex_.Get(cr->id(), &wrapper)){
        {
            std::lock_guard<std::mutex> wl_lg(cr_wl_mtx_);
            if(!id_map_mutex_.Get(cr->id(), &wrapper)){
                wrapper = new MutexWrapper();
                id_map_mutex_.Set(cr->id(), wrapper);
            }
        }
    }

    std::lock_guard<std::mutex> lg(wrapper->Mutex());

    {
        WriteLockGuard<AtomicRWLock> lk(id_cr_lock_);
        if(id_cr_.find(cr->id()) != id_cr_.end()){
            return false;
        }

        id_cr_[cr->id()] = cr;
    }

    if(cr_confs_.find(cr->name()) != cr_confs_.end()){
        ClassicTask task = cr_confs_[cr->name()];
        cr->set_priority(task.prio);
        cr->set_group_name(task.group_name);
    } else {
        cr->set_group_name(classic_conf_.groups[0].name);
    }

    if(cr->priority() >= MAX_PRIO){
        AWARN << cr->name() << " prio is greater than MAX_PRIO[ << " << MAX_PRIO
              << "].";
        cr->set_priority(MAX_PRIO - 1);
    }

    // 将协程放入对应的优先级队列
    {
        WriteLockGuard<AtomicRWLock> lk(
            ClassicContext::rq_locks_[cr->group_name()].at(cr->priority()));
        ClassicContext::cr_group_[cr->group_name()]
            .at(cr->priority()).emplace_back(cr);
    }

    // 唤醒协程所属的group内的processor执行任务
    ClassicContext::Notify(cr->group_name());
    return true;
}

bool SchedulerClassic::NotifyProcessor(uint64_t crid) {
    if(cyber_likely(stop_)){
        return true;
    }

    {
        ReadLockGuard<AtomicRWLock> lk(id_cr_lock_);
        if(id_cr_.find(crid) != id_cr_.end()){
            auto cr = id_cr_[crid];
            if(cr->state() == RoutineState::DATA_WAIT || 
               cr->state() == RoutineState::IO_WAIT ){
               cr->SetUpdateFlag();
            }

            ClassicContext::Notify(cr->group_name());
            return true;
        }
    }
    return false;
}

bool SchedulerClassic::RemoveTask(const std::string& name) {
  if (cyber_unlikely(stop_)) {
    return true;
  }

  auto crid = GlobalData::GenerateHashId(name);
  return RemoveCRoutine(crid);
}

bool SchedulerClassic::RemoveCRoutine(uint64_t crid){
    MutexWrapper* wrapper = nullptr;

    if (!id_map_mutex_.Get(crid, &wrapper)) {
    {
      std::lock_guard<std::mutex> wl_lg(cr_wl_mtx_);
      if (!id_map_mutex_.Get(crid, &wrapper)) {
        wrapper = new MutexWrapper();
        id_map_mutex_.Set(crid, wrapper);
      }
    }
  }

  std::lock_guard<std::mutex> lg(wrapper->Mutex());
  std::shared_ptr<CRoutine> cr = nullptr;
  {
    WriteLockGuard<AtomicRWLock> lk(id_cr_lock_);
    if (id_cr_.find(crid) != id_cr_.end()) {
      cr = id_cr_[crid];
      id_cr_[crid]->Stop();
      id_cr_.erase(crid);
    } else {
      return false;
    }    
  }

  return ClassicContext::RemoveCRoutine(cr);
}

}
}
}