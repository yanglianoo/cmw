#ifndef CMW_COMMON_MACROS_H_
#define CMW_COMMON_MACROS_H_

#include <iostream>
#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>

#include <cmw/base/macros.h>


DEFINE_TYPE_TRAIT(HasShutdown, Shutdown)

template <typename T>
typename std::enable_if<HasShutdown<T>::value>::type CallShutdown(T *instance){
    instance->Shutdown();
}

template <typename T>
typename std::enable_if<!HasShutdown<T>::value>::type CallShutdown(T *instance){
    (void)instance;
}

/*禁用拷贝构造和 = 赋值*/
#define DISALLOW_COPY_AND_ASSIGN(classname) \
  classname(const classname &) = delete;    \
  classname &operator=(const classname &) = delete;

/*利用 std::once_flag 和 std::call_once 实现的线程安全的单例宏*/
#define DECLARE_SINGLETON(classname)                                      \
 public:                                                                  \
  static classname *Instance(bool create_if_needed = true) {              \
    static classname *instance = nullptr;                                 \
    if (!instance && create_if_needed) {                                  \
      static std::once_flag flag;                                         \
      std::call_once(flag,                                                \
                     [&] { instance = new (std::nothrow) classname(); }); \
    }                                                                     \
    return instance;                                                      \
  }                                                                       \
                                                                          \
  static void CleanUp() {                                                 \
    auto instance = Instance(false);                                      \
    if (instance != nullptr) {                                            \
      CallShutdown(instance);                                             \
    }                                                                     \
  }                                                                       \
                                                                          \
 private:                                                                 \
  classname();                                                            \
  DISALLOW_COPY_AND_ASSIGN(classname)


#endif