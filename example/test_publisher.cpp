#include <cmw/node/publisher.h>
#include <cmw/config/RoleAttributes.h>
#include <cmw/common/global_data.h>
#include <cmw/serialize/serializable.h>
#include <cmw/serialize/data_stream.h>
#include <vector>
using namespace hnu::cmw;


struct TestMsg : public Serializable
{
    uint64_t timestamp;  

    std::vector<int> image;
    SERIALIZE(timestamp,image)
};

int main()
{
    Logger_Init("test.log");
    config::RoleAttributes role_attr;

    role_attr.channel_name = "/chatter0";
    role_attr.node_name = "publisher";
    role_attr.channel_id =common::GlobalData::RegisterChannel("/chatter0");
    
    std::cout<<boolalpha;
    int n = 0;
    TestMsg testmsg;
    testmsg.timestamp = 0;
    testmsg.image.resize(1920*1080*3,0);

    //设置发送数据的大小
    serialize::DataStream ds; 
    ds << testmsg;
    role_attr.qos_profile.msg_size= ds.size();

    Publisher<TestMsg> publisher(role_attr);
    std::cout<<"Init publisher " << publisher.Init() << std::endl;

    std::shared_ptr<TestMsg> msg_ptr = std::make_shared<TestMsg>(testmsg);
    while (1)
    {
       publisher.Publish(msg_ptr);
       std::cout << "Publisher seq: " << n << std::endl;
       std::this_thread::sleep_for(std::chrono::milliseconds(250));
       n++;
    }
    
    return 0;
}