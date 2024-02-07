
#include <cmw/transport/shm/notifier_factory.h>
#include <cmw/transport/shm/condition_notifier.h>
#include <cmw/transport/shm/multicast_notifier.h>
#include <string>
#include <cmw/common/global_data.h>

namespace hnu{
namespace cmw{
namespace transport{

using common::GlobalData;

auto NotifierFactory::CreateNotifier() -> NotifierPtr{

    //默认为共享内存的方式
    std::string notifier_type(ConditionNotifier::Type());
    //需要根据配置文件传入Notifier type
    if (notifier_type == MulticastNotifier::type() ){
        return CreateMulticastNotifier();
    } else if (notifier_type == ConditionNotifier::Type()) {
        return CreateConditionNotifier();
    }

    std::cout << "unknown notifier, we use default notifier: " << notifier_type << std::endl;
    return CreateConditionNotifier();
}

auto NotifierFactory::CreateConditionNotifier() -> NotifierPtr{
    return ConditionNotifier::Instance();
}

auto NotifierFactory::CreateMulticastNotifier() -> NotifierPtr {
    return MulticastNotifier::Instance();
}


}
}
}