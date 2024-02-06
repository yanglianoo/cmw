#ifndef CMW_TRANSPORT_SHM_NOTIFIER_BASE_H_
#define CMW_TRANSPORT_SHM_NOTIFIER_BASE_H_

#include <cmw/transport/shm/readable_info.h>

namespace hnu{
namespace cmw{
namespace transport{

class NotifierBase;
using NotifierPtr = NotifierBase*;
class NotifierBase{
    
public:
    virtual ~NotifierBase() = default;

    virtual void Shutdown() = 0;
    virtual bool Notify(const ReadableInfo& info) = 0;
    virtual bool Listen(int timeout_ms, ReadableInfo* info) = 0;
};

}
}
}

#endif