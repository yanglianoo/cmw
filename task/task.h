#ifndef CMW_TASK_TASK_H_
#define CMW_TASK_TASK_H_

#include <future>
#include <cmw/task/task_manager.h>

namespace hnu {
namespace cmw {

//在模板函数中不知道传入的参数是左值还是右值，因此使用完美转发
template <typename F, typename... Args>
static auto Async(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>{
    return TaskManager::Instance()->Enqueue(std::forward<F>(f),
                                            std::forward<Args>(args)...);
}


static inline void Yield() {
  if (croutine::CRoutine::GetCurrentRoutine()) {
    croutine::CRoutine::Yield();
  } else {
    std::this_thread::yield();
  }
}

template <typename Rep, typename Period>
static void SleepFor(const std::chrono::duration<Rep, Period>& sleep_duration) {
  auto routine = croutine::CRoutine::GetCurrentRoutine();
  if (routine == nullptr) {
    std::this_thread::sleep_for(sleep_duration);
  } else {
    routine->Sleep(sleep_duration);
  }
}

static inline void USleep(useconds_t usec) {
  auto routine = croutine::CRoutine::GetCurrentRoutine();
  if (routine == nullptr) {
    std::this_thread::sleep_for(std::chrono::microseconds{usec});
  } else {
    routine->Sleep(croutine::Duration(usec));
  }
}

}
}

#endif