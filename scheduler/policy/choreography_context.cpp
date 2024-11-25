// #include <limits>
// #include <unordered_map>
// #include <utility>
// #include <vector>

// #include <cmw/scheduler/policy/choreography_context.h>
// #include <cmw/common/types.h>

// namespace hnu    {
// namespace cmw   {
// namespace scheduler {

// using hnu::cmw::base::ReadLockGuard;
// using hnu::cmw::base::WriteLockGuard;

// using hnu::cmw::croutine::RoutineState;


// std::shared_ptr<CRoutine> ChoreographyContext::NextRoutine() {
//     if(cyber_likely(stop_.load())){
//         return nullptr;
//     }

//     ReadLockGuard<AtomicRWLock> lock(rq_lk_);

//     for(auto it : cr_queue_){
//         auto cr = it.second;
//         if(!cr->Acquire()){
//             continue;
//         }

//         if(cr->UpdateState() == RoutineState::READY){
//             return cr;
//         }
//         cr->Release();
//     }

//     return nullptr;
// }

// bool ChoreographyContext::Enqueue(const std::shared_ptr<CRoutine>& cr){
//     WriteLockGuard<AtomicRWLock> lock(rq_lk_);
//     cr_queue_.emplace(cr->priority(), cr);
//     return true;
// }

// void ChoreographyContext::Notify(){
//     mtx_wq_.lock();
//     notify++;
//     mtx_wq_.unlock();
//     cv_wq_.notify_one();
// }

// void ChoreographyContext::Wait(){
//     std::unique_lock<std::mutex> lk(mtx_wq_);
//     cv_wq_.wait_for(lk, std::chrono::milliseconds(1000),
//                     [&]() { return notify > 0; } );
//     if(notify > 0){
//         notify--;
//     }
// }

// bool ChoreographyContext::RemoveCRoutine(uint64_t crid){
//     WriteLockGuard<AtomicRWLock> lock(rq_lk_);
//     for(auto it = cr_queue_.begin(); it != cr_queue_.end();){
//         auto cr = it->second;
//         if(cr->id() == crid){
//             cr->Stop();
//             while (!cr->Acquire())
//             {
//                 std::this_thread::sleep_for(std::chrono::milliseconds(1));
//                 AINFO << "waiting for task " << cr->name() << " completion";
//             }
//             it = cr_queue_.erase(it);
//             cr->Release();
//             return true;
//         }
//         ++it;
//     }
//     return false;
// }

// }
// }
// }