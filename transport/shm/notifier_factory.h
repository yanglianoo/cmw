
#ifndef CMW_TRANSPORT_SHM_NOTIFIER_FACTORY_H_
#define CMW_TRANSPORT_SHM_NOTIFIER_FACTORY_H_


#include <memory>
#include <cmw/transport/shm/notifier_base.h>


namespace hnu{
namespace cmw{
namespace transport{


class NotifierFactory{
    public:
        static NotifierPtr CreateNotifier();
    private:
        static NotifierPtr CreateConditionNotifier();
        static NotifierPtr CreateMulticastNotifier();
};


}
}
}



#endif