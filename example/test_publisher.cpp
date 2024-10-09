#include <cmw/node/publisher.h>
#include <cmw/config/RoleAttributes.h>
#include <cmw/common/global_data.h>
using namespace hnu::cmw;

int main()
{
    Logger_Init("test.log");
    config::RoleAttributes role_attr;

    role_attr.channel_name = "/chatter0";
    role_attr.node_name = "publisher";
    role_attr.channel_id =common::GlobalData::RegisterChannel("/chatter0");
    Publisher<string> publisher(role_attr);
    std::cout<<boolalpha;

    std::cout<<"Init publisher " << publisher.Init() << std::endl;
    
    int n = 0;
    while (1)
    {
       std::shared_ptr<std::string> msg_ptr = std::make_shared<std::string>("hnu cmw designed by timer!" + std::to_string(n));
       publisher.Publish(msg_ptr);
       std::cout << "Publisher seq: " << n << std::endl;
       std::this_thread::sleep_for(std::chrono::milliseconds(250));
       n++;
    }
    
    return 0;
}