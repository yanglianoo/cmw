#include <cmw/common/global_data.h>
#include <cmw/config/RoleAttributes.h>
#include <cmw/transport/dispatcher/rtps_dispatcher.h>
#include <cmw/common/util.h>
#include <cmw/transport/receiver/receiver.h>
#include <cmw/transport/transport.h>
#include <cmw/config/topology_change.h>
using namespace hnu::cmw::transport;
using namespace hnu::cmw::common;
using namespace hnu::cmw::config;

using ReceiverPtr = std::shared_ptr<Receiver<std::string>>;



void TEST_MUTILISTENER()
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
    
    auto listener1 = [](const std::shared_ptr<std::string>& message ,
                       const MessageInfo& info, const RoleAttributes&){
                        
                        std::cout << *message << " data1" <<" seq: " << info.seq_num() <<std::endl;
                        
                       };
    auto listener2 = [](const std::shared_ptr<std::string>& message ,
                       const MessageInfo&, const RoleAttributes&){
                        
                        std::cout << *message << " data2" << std::endl;
                        
                       };
    ReceiverPtr rtps1 =Transport::Instance()->CreateReceiver<std::string>(attr,listener1);

    ReceiverPtr rtps2 = Transport::Instance()->CreateReceiver<std::string>(attr,listener2);
    
    printf("Press Enter to stop the Reader.\n");
    std::cin.ignore();
}

void TEST_ChangeMsg()
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

    auto listener1 = [](const std::shared_ptr<ChangeMsg>& message ,
                       const MessageInfo& info, const RoleAttributes&){
                        
                        std::cout<<"time: " << message->timestamp << "operate_type:"  << message->operate_type << "seq:" << info.seq_num() << std::endl;
                        
                       };
    auto rtps1 =Transport::Instance()->CreateReceiver<ChangeMsg>(attr,listener1);
    printf("Press Enter to stop the Reader.\n");
    std::cin.ignore();
}   

int main()
{
    TEST_ChangeMsg();
    //TEST_MUTILISTENER();
    return 0;
}
