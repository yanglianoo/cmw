#include <cmw/node/subscriber.h>
#include <cmw/config/RoleAttributes.h>
#include <cmw/transport/message/message_info.h>
using namespace hnu::cmw;

struct TestMsg : public Serializable
{
    uint64_t timestamp;  

    std::vector<char> image;
    SERIALIZE(timestamp,image)
};

namespace george
{
    void test_rtps_sub(void)
    {
        config::RoleAttributes role_attr;
        role_attr.channel_name = "/chatter0";
        role_attr.node_name = "subscriber";
        role_attr.channel_id =common::GlobalData::RegisterChannel("/chatter0");


        Subscriber<TestMsg> subscriber(
                role_attr,[](const std::shared_ptr<TestMsg>& msg){
                    std::cout << "timestamp is "<< msg->timestamp << std::endl;
                });

        std::boolalpha;

        std::cout<<"Init subscriber " << subscriber.Init() << std::endl;

        while (1)
        {
            /* code */
        }
    }
}

int main()
{
    george::test_rtps_sub();
    
    return 0;
}
