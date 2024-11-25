#ifndef CMW_SCHEDULER_COMMON_MUTEX_WRAPPER_H_
#define CMW_SCHEDULER_COMMON_MUTEX_WRAPPER_H_

#include <mutex>

namespace hnu    {
namespace cmw   {
namespace scheduler {

class MutexWrapper {
 public:
  MutexWrapper& operator=(const MutexWrapper& other) = delete;
  std::mutex& Mutex() { return mutex_; }

 private:
  mutable std::mutex mutex_;
};


}
}
}

#endif