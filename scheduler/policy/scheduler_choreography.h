// #ifndef CMW_SCHEDULER_POLICY_SCHEDULER_CHOREOGRAPHY_H_
// #define CMW_SCHEDULER_POLICY_SCHEDULER_CHOREOGRAPHY_H_

// #include <memory>
// #include <string>
// #include <unordered_map>
// #include <vector>

// #include <cmw/croutine/croutine.h>
// #include <cmw/scheduler/scheduler.h>
// #include <cmw/config/choreography_conf.h>

// namespace hnu    {
// namespace cmw   {
// namespace scheduler {

// using hnu::cmw::croutine::CRoutine;
// using hnu::cmw::config::ChoreographyTask;

// class SchedulerChoreography : public Scheduler {

// public:
//     bool RemoveCRoutine(uint64_t crid) override;
//     bool RemoveTask(const std::string& name) override;
//     bool DispatchTask(const std::shared_ptr<CRoutine>&) override;
    
// private:

//     friend Scheduler* Instance();
//     SchedulerChoreography();

//     void CreateProcessor();
//     bool NotifyProcessor(uint64_t crid) override;


//     std::unordered_map<std::string, ChoreographyTask> cr_confs_;

//     int32_t choreography_processor_prio_;
//     int32_t pool_processor_prio_;

//     std::string choreography_affinity_;
//     std::string pool_affinity_;

//     std::string choreography_processor_policy_;
//     std::string pool_processor_policy_;

//     std::vector<int> choreography_cpuset_;
//     std::vector<int> pool_cpuset_;
// };

// }
// }
// }

// #endif