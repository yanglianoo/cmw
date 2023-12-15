#include <cmw/common/global_data.h>
#include <cmw/config/RoleAttributes.h>
#include <cmw/transport/transport.h>
#include <cmw/transport/message/message_info.h>

void TEST_GLOBAL_DATA()
{
    using namespace hnu::cmw::common;
    std::cout <<"--------------------------Global_Data Test--------------------------" << std::endl;
    std::cout <<"HostIP: " << GlobalData::Instance()->HostIp() << std::endl;
    std::cout <<"HostName: " << GlobalData::Instance()->HostName() << std::endl;
    std::cout <<"ProcessId: " << GlobalData::Instance()->ProcessId() << std::endl;
    std::cout <<"ProcessGroup: " << GlobalData::Instance()->ProcessGroup() << std::endl;
}

void TEST_TRANSPORT_RtpsTransmitter()
{
    using namespace hnu::cmw::config;
    using namespace hnu::cmw::common;
    using namespace hnu::cmw::transport;
    std::cout <<"---------------------Transport Transmitter Test---------------------" << std::endl;
    RoleAttributes attr;
    attr.channel_name = "exampleTopic";
    attr.host_name = GlobalData::Instance()->HostName();
    attr.host_ip = GlobalData::Instance()->HostIp();
    attr.process_id =  GlobalData::Instance()->ProcessId();
    attr.channel_id = GlobalData::Instance()->RegisterChannel(attr.channel_name);
    QosProfile qos;
    attr.qos_profile = qos;

    auto transmitter = Transport::Instance()->CreateTransmitter<std::string>(attr);
    std::string a = "hello cmw";
    std::shared_ptr<std::string> msg_ptr = std::make_shared<std::string>("hello cmw");
    MessageInfo msg;
    uint64_t n = 0;
    
    while (1)
    {

        msg.set_seq_num(n);
        std::cout<<"seq: " << n << std::endl;
        n++;
        transmitter->Transmit(msg_ptr, msg);
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
    

}


int main()
{
    TEST_GLOBAL_DATA();
    TEST_TRANSPORT_RtpsTransmitter();
    return 0;
}
