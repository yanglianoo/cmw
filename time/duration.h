#ifndef CMW_TIME_DURATION_H_
#define CMW_TIME_DURATION_H_
#include <cstdint>
#include <iostream>

namespace hnu {
namespace cmw {

class Duration {
 public:
  Duration() = default;
  explicit Duration(int64_t nanoseconds);
  explicit Duration(int nanoseconds);
  explicit Duration(double seconds);
  Duration(uint32_t seconds, uint32_t nanoseconds);
  Duration(const Duration &other);
  Duration &operator=(const Duration &other);
  ~Duration() = default;

  double ToSecond() const;
  int64_t ToNanosecond() const;
  bool IsZero() const;
  void Sleep() const;

  Duration operator+(const Duration &rhs) const;
  Duration operator-(const Duration &rhs) const;
  Duration operator-() const;
  Duration operator*(double scale) const;
  Duration &operator+=(const Duration &rhs);
  Duration &operator-=(const Duration &rhs);
  Duration &operator*=(double scale);
  bool operator==(const Duration &rhs) const;
  bool operator!=(const Duration &rhs) const;
  bool operator>(const Duration &rhs) const;
  bool operator<(const Duration &rhs) const;
  bool operator>=(const Duration &rhs) const;
  bool operator<=(const Duration &rhs) const;

 private:
  //纳秒
  int64_t nanoseconds_ = 0;
};

}
}



#endif