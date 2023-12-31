#ifndef CMW_TIME_RATE_H_
#define CMW_TIME_RATE_H_


#include <cmw/time/time.h>
#include <cmw/time/duration.h>

namespace hnu {
namespace cmw {

class Rate {
 public:
  explicit Rate(double frequency);
  explicit Rate(uint64_t nanoseconds);
  explicit Rate(const Duration&);
  void Sleep();
  void Reset();
  Duration CycleTime() const;
  Duration ExpectedCycleTime() const { return expected_cycle_time_; }

 private:
  Time start_;
  Duration expected_cycle_time_;
  Duration actual_cycle_time_;
};


}
}


#endif