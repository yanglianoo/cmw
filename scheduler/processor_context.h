#ifndef CMW_SCHEDULER_POLICY_PROCESSOR_CONTEXT_H_
#define CMW_SCHEDULER_POLICY_PROCESSOR_CONTEXT_H_

#include <limits>
#include <memory>
#include <mutex>

#include <cmw/base/macros.h>
#include <cmw/croutine/croutine.h>

namespace hnu    {
namespace cmw   {
namespace scheduler {

using croutine::CRoutine;

class ProcessorContext {
 public:
  virtual void Shutdown();
  virtual std::shared_ptr<CRoutine> NextRoutine() = 0;
  virtual void Wait() = 0;

 protected:
  std::atomic<bool> stop_{false};
};

}
}
}

#endif