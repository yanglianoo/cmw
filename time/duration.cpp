#include <cmw/time/duration.h>
#include <thread>

namespace hnu {
namespace cmw {

Duration::Duration(int64_t nanoseconds) { nanoseconds_ = nanoseconds; }
Duration::Duration(int nanoseconds) {
  nanoseconds_ = static_cast<int64_t>(nanoseconds);
}

Duration::Duration(double seconds) {
  nanoseconds_ = static_cast<int64_t>(seconds * 1000000000UL);
}

Duration::Duration(uint32_t seconds, uint32_t nanoseconds) {
  nanoseconds_ = static_cast<uint64_t>(seconds) * 1000000000UL + nanoseconds;
}

Duration::Duration(const Duration &other) { nanoseconds_ = other.nanoseconds_; }

Duration &Duration::operator=(const Duration &other) {
  this->nanoseconds_ = other.nanoseconds_;
  return *this;
}

double Duration::ToSecond() const {
  return static_cast<double>(nanoseconds_) / 1000000000UL;
}

int64_t Duration::ToNanosecond() const { return nanoseconds_; }

bool Duration::IsZero() const { return nanoseconds_ == 0; }

void Duration::Sleep() const {
  auto sleep_time = std::chrono::nanoseconds(nanoseconds_);
  std::this_thread::sleep_for(sleep_time);
}

Duration Duration::operator+(const Duration &rhs) const {
  return Duration(nanoseconds_ + rhs.nanoseconds_);
}

Duration Duration::operator-(const Duration &rhs) const {
  return Duration(nanoseconds_ - rhs.nanoseconds_);
}

Duration Duration::operator-() const { return Duration(-nanoseconds_); }

Duration Duration::operator*(double scale) const {
  return Duration(int64_t(static_cast<double>(nanoseconds_) * scale));
}

Duration &Duration::operator+=(const Duration &rhs) {
  *this = *this + rhs;
  return *this;
}

Duration &Duration::operator-=(const Duration &rhs) {
  *this = *this - rhs;
  return *this;
}

Duration &Duration::operator*=(double scale) {
  *this = Duration(int64_t(static_cast<double>(nanoseconds_) * scale));
  return *this;
}

bool Duration::operator==(const Duration &rhs) const {
  return nanoseconds_ == rhs.nanoseconds_;
}

bool Duration::operator!=(const Duration &rhs) const {
  return nanoseconds_ != rhs.nanoseconds_;
}

bool Duration::operator>(const Duration &rhs) const {
  return nanoseconds_ > rhs.nanoseconds_;
}

bool Duration::operator<(const Duration &rhs) const {
  return nanoseconds_ < rhs.nanoseconds_;
}

bool Duration::operator>=(const Duration &rhs) const {
  return nanoseconds_ >= rhs.nanoseconds_;
}

bool Duration::operator<=(const Duration &rhs) const {
  return nanoseconds_ <= rhs.nanoseconds_;
}



}
}