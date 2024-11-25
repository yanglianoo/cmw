

#ifndef CMW_SCHEDULER_COMMON_PIN_THREAD_H_
#define CMW_SCHEDULER_COMMON_PIN_THREAD_H_

#include <string>
#include <thread>
#include <vector>

#include <cmw/common/log.h>

namespace hnu    {
namespace cmw   {
namespace scheduler {

void ParseCpuset(const std::string& str, std::vector<int>* cpuset);

void SetSchedAffinity(std::thread* thread, const std::vector<int>& cpus,
                      const std::string& affinity, int cpu_id = -1);

void SetSchedPolicy(std::thread* thread, std::string spolicy,
                    int sched_priority, pid_t tid = -1);                     

}
}
}

#endif