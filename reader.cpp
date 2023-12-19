#include <cmw/common/global_data.h>
#include <cmw/config/RoleAttributes.h>
#include <cmw/transport/dispatcher/rtps_dispatcher.h>
#include <cmw/common/util.h>
#include <cmw/transport/receiver/receiver.h>
#include <cmw/transport/transport.h>
using namespace hnu::cmw::transport;
using namespace hnu::cmw::common;
using ReceiverPtr = std::shared_ptr<Receiver<std::string>>;
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
    
    auto listener = [](const std::shared_ptr<std::string>& message ,
                       const MessageInfo&, const RoleAttributes&){
                        
                        std::cout << *message << std::endl;
                       };

    ReceiverPtr rtps =Transport::Instance()->CreateReceiver<std::string>(attr,listener);
    
    printf("Press Enter to stop the Reader.\n");
    std::cin.ignore();
    return 0;
}
