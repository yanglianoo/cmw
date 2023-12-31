#ifndef CMW_BASE_MACROS_H_
#define CMW_BASE_MACROS_H_


#include <cstdlib>
#include <new>

#if __GNUC__ >= 3
#define cyber_likely(x) (__builtin_expect((x), 1))
#define cyber_unlikely(x) (__builtin_expect((x), 0))
#else
#define cyber_likely(x) (x)
#define cyber_unlikely(x) (x)
#endif

#define CACHELINE_SIZE 64

// 创建一个名为name的类用于判断 T 中是否含有 func 函数
#define DEFINE_TYPE_TRAIT(name, func)                     \
  template <typename T>                                   \
  struct name {                                           \
    template <typename Class>                             \
    static constexpr bool Test(decltype(&Class::func)*) { \
      return true;                                        \
    }                                                     \
    template <typename>                                   \
    static constexpr bool Test(...) {                     \
      return false;                                       \
    }                                                     \
                                                          \
    static constexpr bool value = Test<T>(nullptr);       \
  };                                                      \
                                                          \
  template <typename T>                                   \
  constexpr bool name<T>::value;



#if !defined(RETURN_IF_NULL)
#define RETURN_IF_NULL(ptr)          \
  if (ptr == nullptr) {              \
    std::cout << #ptr << " is nullptr."<< std::endl; \
    return;                          \
  }
#endif


#if !defined(RETURN_VAL_IF_NULL)
#define RETURN_VAL_IF_NULL(ptr, val) \
  if (ptr == nullptr) {              \
    std::cout << #ptr << " is nullptr."<< std::endl; \
    return val;                      \
  }
#endif

#if !defined(RETURN_VAL_IF)
#define RETURN_VAL_IF(condition, val)  \
  if (condition) {                     \
    std::cout << #condition << " is met."<< std::endl; \
    return val;                        \
  }
#endif

#endif