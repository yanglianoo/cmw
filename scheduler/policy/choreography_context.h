// #ifndef CMW_SCHEDULER_POLICY_CHOREOGRAPHY_CONTEXT_H_
// #define CMW_SCHEDULER_POLICY_CHOREOGRAPHY_CONTEXT_H_

// #include <functional>
// #include <map>
// #include <memory>
// #include <mutex>
// #include <string>
// #include <unordered_map>

// #include <cmw/base/atomic_rw_lock.h>
// #include <cmw/croutine/croutine.h>
// #include <cmw/scheduler/processor_context.h>


// namespace hnu    {
// namespace cmw   {
// namespace scheduler {

// using hnu::cmw::base::AtomicRWLock;
// using croutine::CRoutine;

// class ChoreographyContext : public ProcessorContext{

// public:
//     bool RemoveCRoutine(uint64_t crid);
//     std::shared_ptr<CRoutine> NextRoutine() override;

//     bool Enqueue(const std::shared_ptr<CRoutine>&);
//     void Notify();
//     void Wait() override;
//     void Shutdown() override;

// private:
//     std::mutex mtx_wq_;
//     std::condition_variable cv_wq_;
//     int notify = 0;

//     AtomicRWLock rq_lk_;
//     std::multimap<uint32_t, std::shared_ptr<CRoutine>, std::greater<uint32_t>>
//         cr_queue_;
// };

// }
// }
// }
// #endif