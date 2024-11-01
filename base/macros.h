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

// malloc 分配内存不会初始化
inline void* CheckedMalloc(size_t size){
  void* ptr = std::malloc(size);
  if(!ptr){
    throw std::bad_alloc();
  }
  return ptr;
}

// calloc 分配num*size的内存，全部初始化为0
inline void* CheckedCalloc(size_t num, size_t size){
  void* ptr = std::calloc(num, size);
  if(!ptr){
    throw std::bad_alloc();
  }
  return ptr;
}


#endif