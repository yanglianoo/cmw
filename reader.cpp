#include <cmw/common/global_data.h>
#include <cmw/config/RoleAttributes.h>
#include <cmw/transport/dispatcher/rtps_dispatcher.h>
#include <cmw/common/util.h>
using namespace hnu::cmw::transport;
using namespace hnu::cmw::common;
int main()
{
    auto dispatcher = RtpsDispatcher::Instance();
    RoleAttributes attr;
    attr.channel_name = "exampleTopic";
    attr.host_name = GlobalData::Instance()->HostName();
    attr.host_ip = GlobalData::Instance()->HostIp();
    attr.process_id =  GlobalData::Instance()->ProcessId();
    attr.channel_id = GlobalData::Instance()->RegisterChannel(attr.channel_name);
    QosProfile qos;
    attr.qos_profile = qos;
    
    auto recv_msg = std::make_shared<std::string>();
    dispatcher->AddListener<std::string>(attr,
                [&recv_msg](const std::shared_ptr<std::string>& msg,
                             const MessageInfo& msg_info)
                             {
                                (void)msg_info;
                                std::cout<<"rev data" << std::endl;
                             });
    
    printf("Press Enter to stop the Reader.\n");
    std::cin.ignore();
    return 0;
}
