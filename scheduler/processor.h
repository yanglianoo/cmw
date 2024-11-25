#ifndef CMW_SCHEDULER_PROCESSOR_H_
#define CMW_SCHEDULER_PROCESSOR_H_

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <cmw/config/scheduler_conf.h>
#include <cmw/croutine/croutine.h>
#include <cmw/scheduler/processor_context.h>

namespace hnu    {
namespace cmw   {
namespace scheduler {

using croutine::CRoutine;

struct Snapshot {
  std::atomic<uint64_t> execute_start_time = {0};
  std::atomic<pid_t> processor_id = {0};
  std::string routine_name;
};

class Processor {

public:
    Processor();
    virtual ~Processor();

    void Run();
    void Stop();
    void BindContext(const std::shared_ptr<ProcessorContext>& context);
    std::thread* Thread() { return &thread_; }

    std::atomic<pid_t>& Tid();
    std::shared_ptr<Snapshot> ProcSnapshot() { return snap_shot_; }
    
private:
    std::shared_ptr<ProcessorContext> context_;

    std::condition_variable cv_ctx_;
    std::once_flag thread_flag_;
    std::mutex mtx_ctx_;
    std::thread thread_;

    std::atomic<pid_t> tid_{-1};
    std::atomic<bool> running_{false};

    std::shared_ptr<Snapshot> snap_shot_ = std::make_shared<Snapshot>();
};
}
}
}

#endif