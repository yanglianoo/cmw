#ifndef CMW_TASK_TASK_H_
#define CMW_TASK_TASK_H_

#include <future>
namespace hnu {
namespace cmw {

template <typename F, typename... Args>
static auto Async(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>{
  return 
}

}
}

#endif