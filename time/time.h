#ifndef CMW_TIME_TIME_H_
#define CMW_TIME_TIME_H_

#include <cmw/time/duration.h>
namespace hnu {
namespace cmw {

//explicit 用于防止隐式转换
class Time
{
public:
    static const Time MAX;
    static const Time MIN;
    Time() = default;
    explicit Time(uint64_t nanoseconds);
    explicit Time(int nanoseconds);
    explicit Time(double seconds);
    Time(u_int32_t seconds, u_int32_t nanoseconds);
    Time(const Time& other);
    Time& operator=(const Time& other);

    static Time Now();
    static Time MonoTime();
    static void SleepUntil(const Time& time);
    double ToSecond() const;
    uint64_t ToMicrosecond() const;
    uint64_t ToNanosecond() const;
    std::string ToString() const;
    bool IsZero() const;

    Duration operator-(const Time& rhs) const;
    Time operator+(const Duration& rhs) const;
    Time operator-(const Duration& rhs) const;
    Time& operator+=(const Duration& rhs);
    Time& operator-=(const Duration& rhs);
    bool operator==(const Time& rhs) const;
    bool operator!=(const Time& rhs) const;
    bool operator>(const Time& rhs) const;
    bool operator<(const Time& rhs) const;
    bool operator>=(const Time& rhs) const;
    bool operator<=(const Time& rhs) const;

private:
    uint64_t nanoseconds_ = 0;
};

std::ostream& operator<<(std::ostream& os, const Time& rhs);



}
}


#endif