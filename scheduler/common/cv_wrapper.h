#ifndef CMW_SCHEDULER_COMMON_CV_WRAPPER_H_
#define CMW_SCHEDULER_COMMON_CV_WRAPPER_H_

#include <condition_variable>

namespace hnu    {
namespace cmw   {
namespace scheduler {


class CvWrapper {
    public:
        CvWrapper& operator=(const CvWrapper& other) = delete;
        std::condition_variable& Cv() { return cv_; }
    
    private:
        mutable std::condition_variable cv_;
};


}
}
}

#endif