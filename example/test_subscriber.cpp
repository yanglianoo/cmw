#include <cmw/node/subscriber.h>
#include <cmw/config/RoleAttributes.h>
#include <cmw/transport/message/message_info.h>
using namespace hnu::cmw;

namespace george
{
    void test_rtsp_sub(void)
    {
        config::RoleAttributes role_attr;
        role_attr.channel_name = "/chatter0";
        role_attr.node_name = "subscriber";
        role_attr.channel_id =common::GlobalData::RegisterChannel("/chatter0");


        Subscriber<string> subscriber(
                role_attr,[](const std::shared_ptr<string>& msg){
                    std::cout << *msg << std::endl;
                });

        std::boolalpha;

        std::cout<<"Init publisher " << subscriber.Init() << std::endl;

        while (1)
        {
            /* code */
        }
    }
}


int main()
{

    
    return 0;
}
