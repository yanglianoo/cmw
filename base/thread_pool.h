#ifndef CMW_BASE_THREAD_POOL_H_
#define CMW_BASE_THREAD_POOL_H_


#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <queue>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

#include <cmw/base/bounded_queue.h>
namespace hnu    {
namespace cmw   {
namespace base {

class ThreadPool {
 public:
  explicit ThreadPool(std::size_t thread_num, std::size_t max_task_num = 1000);

  template <typename F, typename... Args>
  auto Enqueue(F&& f, Args&&... args)
      -> std::future<typename std::result_of<F(Args...)>::type>;

  ~ThreadPool();

 private:
  std::vector<std::thread> workers_;
  BoundedQueue<std::function<void()>> task_queue_;
  std::atomic_bool stop_;
};

/*构造函数入参为 线程数量和最大任务数量*/
inline ThreadPool::ThreadPool(std::size_t threads, std::size_t max_task_num)
    : stop_(false) {
  /*创建一个BoundedQueue，采用的等待策略是阻塞策略*/
  if (!task_queue_.Init(max_task_num, new BlockWaitStrategy())) {
    throw std::runtime_error("Task queue init failed.");
  }

  /* 初始化线程池 创建空的任务，每个任务都是一个while循环 */
  workers_.reserve(threads);
  for (size_t i = 0; i < threads; ++i) {
    workers_.emplace_back([this] {
      while (!stop_) {
        /*返回值为空的可调用对象*/
        std::function<void()> task;
        if (task_queue_.WaitDequeue(&task)) {
          /*如果出队成功，说明领取到了任务，则就去执行此任务*/
          task();
        }
      }
    });
  }
}

// before using the return value, you should check value.valid()
template <typename F, typename... Args>
auto ThreadPool::Enqueue(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type> {
  using return_type = typename std::result_of<F(Args...)>::type;

  auto task = std::make_shared<std::packaged_task<return_type()>>(
      std::bind(std::forward<F>(f), std::forward<Args>(args)...));
  
  std::future<return_type> res = task->get_future();

  // don't allow enqueueing after stopping the pool
  if (stop_) {
    return std::future<return_type>();
  }
  task_queue_.Enqueue([task]() { (*task)(); });
  return res;
};

// the destructor joins all threads
/* 唤醒线程池里所有线程，然后等待所有子线程执行完毕，释放资源*/
inline ThreadPool::~ThreadPool() {
  if (stop_.exchange(true)) {
    return;
  }
  task_queue_.BreakAllWait();
  for (std::thread& worker : workers_) {
    worker.join();
  }
}



}
}
}

#endif
