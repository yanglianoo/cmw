#ifndef CMW_SCHEDULER_POLICY_SCHEDULER_CLASSIC_H_
#define CMW_SCHEDULER_POLICY_SCHEDULER_CLASSIC_H_


#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <cmw/croutine/croutine.h>
#include <cmw/scheduler/scheduler.h>
#include <cmw/config/classic_conf.h>

namespace hnu    {
namespace cmw   {
namespace scheduler {

using hnu::cmw::croutine::CRoutine;
using hnu::cmw::config::ClassicConf;
using hnu::cmw::config::ClassicTask;

class SchedulerClassic : public Scheduler {

public:
    bool RemoveCRoutine(uint64_t crid) override;
    bool RemoveTask(const std::string& name) override;
    bool DispatchTask(const std::shared_ptr<CRoutine>&) override;
private:
    friend Scheduler* Instance();
    SchedulerClassic();
    void CreateProcessor();
    bool NotifyProcessor(uint64_t crid) override;
    std::unordered_map<std::string, ClassicTask> cr_confs_;

    ClassicConf classic_conf_;
};


}
}
}
#endif