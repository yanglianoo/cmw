// #include <cmw/scheduler/policy/scheduler_choreography.h>

// #include <memory>
// #include <string>
// #include <utility>

// #include <cmw/common/environment.h>
// #include <cmw/common/file.h>
// #include <cmw/scheduler/policy/choreography_context.h>
// #include <cmw/scheduler/policy/classic_context.h>
// #include <cmw/scheduler/processor.h>
// #include <cmw/config/cmw_conf.h>

// namespace hnu    {
// namespace cmw   {
// namespace scheduler {


// using hnu::cmw::base::AtomicRWLock;
// using hnu::cmw::base::ReadLockGuard;
// using hnu::cmw::base::WriteLockGuard;
// using hnu::cmw::common::GetAbsolutePath;

// using hnu::cmw::common::GlobalData;
// using hnu::cmw::common::PathExists;
// using hnu::cmw::common::WorkRoot;
// using hnu::cmw::croutine::RoutineState;

// SchedulerChoreography::SchedulerChoreography()
//         : choreography_processor_prio_(0), pool_processor_prio_(0){
//     std::string conf("conf/");
//     conf.append(GlobalData::Instance()->ProcessGroup()).append(".conf");
//     auto cfg_file = GetAbsolutePath(WorkRoot(), conf);

//     hnu::cmw::config::CmwConfig cfg;
//     if(PathExists(cfg_file)){
//         for(auto& thr : cfg.scheduler_conf.threads){
//             inner_thr_confs_[thr.name] = thr;
//         }

//         if(!cfg.scheduler_conf.process_level_cpuset.empty()){
//             process_level_cpuset_ = cfg.scheduler_conf.process_level_cpuset;
//         }
//     }

// }

// }
// }
// }