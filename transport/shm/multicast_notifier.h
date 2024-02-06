#ifndef CMW_TRANSPORT_SHM_MULTICAST_NOTIFIER_H_
#define CMW_TRANSPORT_SHM_MULTICAST_NOTIFIER_H_

#include <netinet/in.h>
#include <atomic>
#include <cmw/transport/shm/notifier_base.h>
namespace hnu{
namespace cmw{
namespace transport{

class MulticastNnotifier : public NotifierBase
{

private:
    bool Init();
    int notify_id_ = -1;
    struct sockaddr_in notify_addr_;
};





}
}
}


#endif