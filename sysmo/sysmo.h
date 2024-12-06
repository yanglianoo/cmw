#ifndef CMW_SYSMO_SYSMO_H_
#define CMW_SYSMO_SYSMO_H_


#include <chrono>
#include <condition_variable>
#include <list>
#include <mutex>
#include <string>
#include <thread>

#include <cmw/scheduler/scheduler_factory.h>

namespace hnu {
namespace cmw {

using hnu::cmw::scheduler::Scheduler;

class SysMo {
 public:
  void Start();
  void Shutdown();

 private:
  void Checker();

  std::atomic<bool> shut_down_{false};
  bool start_ = false;

  int sysmo_interval_ms_ = 100;
  std::condition_variable cv_;
  std::mutex lk_;
  std::thread sysmo_;

  DECLARE_SINGLETON(SysMo);
};

}
}


#endif