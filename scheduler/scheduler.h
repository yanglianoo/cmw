#ifndef CMW_SCHEDULER_SCHEDULER_H_
#define CMW_SCHEDULER_SCHEDULER_H_

#include <unistd.h>

#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include <cmw/config/scheduler_conf.h>
#include <cmw/common/log.h>
#include <cmw/common/macros.h>
#include <cmw/common/types.h>
#include <cmw/croutine/croutine.h>
#include <cmw/croutine/croutine_factory.h>
#include <cmw/scheduler/common/mutex_wrapper.h>
#include <cmw/scheduler/common/pin_thread.h>


namespace hnu    {
namespace cmw   {
namespace scheduler {

using hnu::cmw::base::AtomicHashMap;
using hnu::cmw::base::AtomicRWLock;
using hnu::cmw::base::ReadLockGuard;
using hnu::cmw::croutine::CRoutine;
using hnu::cmw::croutine::RoutineFactory;
using hnu::cmw::data::DataVisitorBase;
using hnu::cmw::config::InnerThread;

class Processor;
class ProcessorContext;

class Scheduler {

public:
    virtual ~Scheduler() {}
    static Scheduler* Instance();

    bool CreateTask(const RoutineFactory& factory, const std::string& name);
    bool CreateTask(std::function<void()>&& func, const std::string& name,
                    std::shared_ptr<DataVisitorBase> visitor = nullptr);
    bool NotifyTask(uint64_t crid);

    void Shutdown();
    uint32_t TaskPoolSize() { return task_pool_size_; }

    virtual bool RemoveTask(const string& name) = 0;

    void ProcessLevelResourceControl();
    void SetInnerThreadAttr(const std::string& name, std::thread* thr);

    virtual bool DispatchTask(const std::shared_ptr<CRoutine>&) = 0;
    virtual bool NotifyProcessor(uint64_t crid) = 0;
    virtual bool RemoveCRoutine(uint64_t crid) = 0;

    void CheckSchedStatus();

    void SetInnerThreadConfs(
        const std::unordered_map<std::string, InnerThread>& confs) {
        inner_thr_confs_ = confs;
    }
    
protected:
    Scheduler() : stop_(false) {}

    AtomicRWLock id_cr_lock_;
    AtomicHashMap<uint64_t, MutexWrapper*> id_map_mutex_;
    std::mutex cr_wl_mtx_;

    std::unordered_map<uint64_t, std::shared_ptr<CRoutine>> id_cr_;
    std::vector<std::shared_ptr<ProcessorContext>> pctxs_;
    std::vector<std::shared_ptr<Processor>> processors_;

    std::unordered_map<std::string, InnerThread> inner_thr_confs_;

    std::string process_level_cpuset_;
    uint32_t proc_num_ = 0;
    uint32_t task_pool_size_ = 0;
    std::atomic<bool> stop_;
};
}
}
}
#endif