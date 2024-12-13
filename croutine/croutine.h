#ifndef CMW_CROUTINE_CROUTINE_H_
#define CMW_CROUTINE_CROUTINE_H_

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <set>
#include <string>

#include <cmw/common/log.h>
#include <cmw/croutine/croutine_context.h>


namespace hnu {
namespace cmw {
namespace croutine{

using RoutineFunc = std::function<void()>;
using Duration = std::chrono::microseconds;

enum class RoutineState { READY, FINISHED, SLEEP, IO_WAIT, DATA_WAIT };

class CRoutine{
    public:
        explicit CRoutine(const RoutineFunc& func);
        virtual ~CRoutine();

        static void Yield();
        static void Yield(const RoutineState& state);
        static void SetMainContext(const std::shared_ptr<RoutineContext>& context);
        static CRoutine *GetCurrentRoutine();
        static char **GetMainStack();

        bool Acquire();
        void Release();

        void SetUpdateFlag();

        
        RoutineState Resume();
        RoutineState UpdateState();
        RoutineContext *GetContext();
        char **GetStack();

        void Run();
        void Stop();
        void Wake();
        void HangUp();
        void Sleep(const Duration& sleep_duration);

        // getter and setter
        RoutineState state() const;
        void set_state(const RoutineState& state);

        uint64_t id() const;
        void set_id(uint64_t id);

        const std::string &name() const;
        void set_name(const std::string &name);

        int processor_id() const;
        void set_processor_id(int processor_id);

        uint32_t priority() const;
        void set_priority(uint32_t priority);

        std::chrono::steady_clock::time_point wake_time() const;

        void set_group_name(const std::string &group_name) {
          group_name_ = group_name;
        }

        const std::string &group_name() { return group_name_; }

    private:
        CRoutine(CRoutine &) = delete;
        CRoutine &operator=(CRoutine &) = delete;

        std::string name_;
        // 获取当前时间
        std::chrono::steady_clock::time_point wake_time_ =
                std::chrono::steady_clock::now();

        RoutineFunc func_;
        RoutineState state_;

        std::shared_ptr<RoutineContext> context_;

        std::atomic_flag lock_ = ATOMIC_FLAG_INIT;
        std::atomic_flag updated_ = ATOMIC_FLAG_INIT;

        bool force_stop_ = false;

        int processor_id_ = -1;
        uint32_t priority_ = 0;
        uint64_t id_ = 0;

        std::string group_name_;
        
        static thread_local CRoutine *current_routine_;
        static thread_local char *main_stack_;
};

inline void CRoutine::Yield(const RoutineState &state) {
    auto routine = GetCurrentRoutine();
    routine->set_state(state);
    SwapContext(GetCurrentRoutine()->GetStack(), GetMainStack());
}

inline void CRoutine::Yield() {
  SwapContext(GetCurrentRoutine()->GetStack(), GetMainStack());
}

// 拿到当前Routine的指针
inline CRoutine *CRoutine::GetCurrentRoutine() { return current_routine_; }

// 获取当前主栈的地址
inline char **CRoutine::GetMainStack() { return &main_stack_; }

inline RoutineContext *CRoutine::GetContext() { return context_.get(); }

inline char **CRoutine::GetStack() { return &(context_->sp); }

inline void CRoutine::Run() { func_(); }

inline void CRoutine::set_state(const RoutineState &state) { state_ = state; }

inline RoutineState CRoutine::state() const { return state_; }

inline std::chrono::steady_clock::time_point CRoutine::wake_time() const {
    return wake_time_;
}

inline void CRoutine::Wake() { state_ = RoutineState::READY; }

inline void CRoutine::HangUp() { CRoutine::Yield(RoutineState::DATA_WAIT); }

inline void CRoutine::Sleep(const Duration &sleep_duration) {
  wake_time_ = std::chrono::steady_clock::now() + sleep_duration;
  CRoutine::Yield(RoutineState::SLEEP);
}

inline uint64_t CRoutine::id() const { return id_; }

inline void CRoutine::set_id(uint64_t id) { id_ = id; }

inline const std::string &CRoutine::name() const { return name_; }

inline void CRoutine::set_name(const std::string &name) { name_ = name; }

inline int CRoutine::processor_id() const { return processor_id_; }

inline void CRoutine::set_processor_id(int processor_id) {
  processor_id_ = processor_id;
}

inline RoutineState CRoutine::UpdateState() {

    if(state_ == RoutineState::SLEEP && 
        std::chrono::steady_clock::now() > wake_time_){
            state_ = RoutineState::READY;
            return state_;
    }
    if(!updated_.test_and_set(std::memory_order_release)){
        if(state_ == RoutineState::DATA_WAIT || state_ == RoutineState::IO_WAIT){
            state_ = RoutineState::READY;
        }
    }
    return state_;
}

inline uint32_t CRoutine::priority() const { return priority_; }

inline void CRoutine::set_priority(uint32_t priority) { priority_ = priority; }

inline bool CRoutine::Acquire() {
  return !lock_.test_and_set(std::memory_order_acquire);
}

inline void CRoutine::Release() {
  return lock_.clear(std::memory_order_release);
}

inline void CRoutine::SetUpdateFlag() {
  updated_.clear(std::memory_order_release);
}

}
}
}

#endif