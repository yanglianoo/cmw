#include <cmw/time/rate.h>

namespace hnu {
namespace cmw {

Rate::Rate(double frequency)
    : start_(Time::Now()),
      expected_cycle_time_(1.0 / frequency),
      actual_cycle_time_(0.0) {}

Rate::Rate(uint64_t nanoseconds)
    : start_(Time::Now()),
      expected_cycle_time_(static_cast<int64_t>(nanoseconds)),
      actual_cycle_time_(0.0) {}

Rate::Rate(const Duration& d)
    : start_(Time::Now()), expected_cycle_time_(d), actual_cycle_time_(0.0) {}

Duration Rate::CycleTime() const { return actual_cycle_time_; }

void Rate::Reset() { start_ = Time::Now(); }

void Rate::Sleep() {
  // 计算期望的循环结束时间
  Time expected_end = start_ + expected_cycle_time_;

  // 获取实际循环结束时间
  Time actual_end = Time::Now();

  // 检测是否发生了时间回退（backward jumps）
  if (actual_end < start_) {
    std::cout << "Detect backward jumps in time" << std::endl;
    expected_end = actual_end + expected_cycle_time_;
  }

  // 计算需要休眠的时间
  Duration sleep_time = expected_end - actual_end;

  // 记录实际循环运行时间
  actual_cycle_time_ = actual_end - start_;

  // 重置循环的起始时间
  start_ = expected_end;

  // 如果实际运行时间超过了期望的循环结束时间
  if (sleep_time < Duration(0.0)) {
    std::cout << "Detect forward jumps in time" << std::endl;
    // 如果发生时间前进（forward jumps），或者循环运行时间超过一个完整周期，重置循环
    if (actual_end > expected_end + expected_cycle_time_) {
      start_ = actual_end;
    }
    // 返回，表示期望的速率未被满足
    return;
  }

  // 休眠直到期望的循环结束时间
  Time::SleepUntil(expected_end);
}


}
}