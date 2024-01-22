#include <cmw/discovery/specific_manager/channel_manager.h>
#include <memory>
#include <cmw/config/RoleAttributes.h>
#include <cmw/common/global_data.h>
#include <cmw/transport/common/identity.h>
using namespace hnu::cmw::discovery;
using namespace hnu::cmw::config;
using namespace hnu::cmw::common;

class ChannelManagerTest{
public:
    ChannelManagerTest( ) : channel_num_(10)
    {
        RoleAttributes role_attr;
        role_attr.host_name = GlobalData::Instance()->HostName();
        role_attr.process_id =  GlobalData::Instance()->ProcessId();

        std::cout << "Add Writer" << '\n';
        for (size_t i = 0; i < channel_num_; i++)
        {
            role_attr.node_name = "node_" + std::to_string(i);
            role_attr.node_id = GlobalData::RegisterNode(role_attr.node_name );
            role_attr.channel_name = "channel_" + std::to_string(i);
            role_attr.channel_id = GlobalData::RegisterChannel(role_attr.channel_name);
            hnu::cmw::transport::Identity id;
            role_attr.id = id.HashValue();
            channel_manager_.Join(role_attr , RoleType::ROLE_WRITER);
        }

        std::cout << "Add Reader" << '\n';
        for (size_t i = 0; i < channel_num_; i++)
        {
            role_attr.node_name = "node_" + std::to_string(i);
            role_attr.node_id = GlobalData::RegisterNode(role_attr.node_name);
            role_attr.channel_name = "channel_" + std::to_string(i);
            role_attr.channel_id = GlobalData::RegisterChannel(role_attr.channel_name);
            hnu::cmw::transport::Identity id;
            role_attr.id = id.HashValue();
            channel_manager_.Join(role_attr , RoleType::ROLE_READER);
        }
        
        
    }

    int channel_num_;
    ChannelManager channel_manager_;
};

void TEST_JOIN()
{
    ChannelManagerTest test;
}

void TEST_GET_CHANNEL_NAMES(){
    ChannelManagerTest test;
    std::vector<std::string> channels;
    test.channel_manager_.GetChannelNames(&channels);
    std::cout<< "channel count: " << channels.size() << std::endl;
}

void TEST_HAS_WRITER_AND_READER()
{
    ChannelManagerTest test;
    std::cout<< boolalpha;
    for (size_t i = 0; i < test.channel_num_; i++)
    {
       std::cout << test.channel_manager_.HasWriter("channel_" + std::to_string(i)) << " ";
    }   

    std::cout << '\n';

    for (size_t i = 0; i < test.channel_num_; i++)
    {
       std::cout << test.channel_manager_.HasReader("channel_" + std::to_string(i)) << " ";
    }

    std::cout << '\n';

}

void TEST_GET_WRITERS_ATTR()
{
    ChannelManagerTest test;
    std::vector<RoleAttributes> writers;
    test.channel_manager_.GetWriters(&writers);
    std::cout << "writer count: " << writers.size() << std::endl;

    writers.clear();
    for (size_t i = 0; i < test.channel_num_; i++)
    {
        test.channel_manager_.GetWritersOfChannel("channel_" + std::to_string(i) , &writers);
        std::cout << "writer count: " << writers.size() << " ";
        writers.clear();
        test.channel_manager_.GetWritersOfNode("node_" + std::to_string(i) , &writers);
        std::cout << "writer count: " << writers.size() << " ";
        writers.clear();
    }


    //一个Node 含有 100个writer
    RoleAttributes role_attr;
    role_attr.host_name = GlobalData::Instance()->HostName();
    role_attr.process_id =  GlobalData::Instance()->ProcessId();
    role_attr.node_name = "node_extra";
    role_attr.node_id = GlobalData::RegisterNode("node_extra");

    for (size_t i = 0; i < 100; i++)
    {
            role_attr.channel_name = "channel_" + std::to_string(i);
            role_attr.channel_id = GlobalData::RegisterChannel(role_attr.channel_name);
            hnu::cmw::transport::Identity id;
            role_attr.id = id.HashValue();
            test.channel_manager_.Join(role_attr , RoleType::ROLE_WRITER);
    }

    writers.clear();
    test.channel_manager_.GetWritersOfNode("node_extra", &writers);
    std::cout <<'\n' <<"writer count: " << writers.size() << '\n';

}

void TEST_GET_READERS_ATTR()
{
    ChannelManagerTest test;
    std::vector<RoleAttributes> readers;
    test.channel_manager_.GetReaders(&readers);
    std::cout << "readers count: " << readers.size() << std::endl;

    readers.clear();
    for (size_t i = 0; i < test.channel_num_; i++)
    {
        test.channel_manager_.GetReadersOfChannel("channel_" + std::to_string(i) , &readers);
        std::cout << "readers count: " << readers.size() << " ";
        readers.clear();
        test.channel_manager_.GetReadersOfNode("node_" + std::to_string(i) , &readers);
        std::cout << "readers count: " << readers.size() << " ";
        readers.clear();
    }

    std::cout << '\n';

    //拿到和channel_0绑定的readers
    test.channel_manager_.GetReadersOfChannel("channel_0" , &readers);
    std::cout << "readers size: " << readers.size() << std::endl;

    RoleAttributes role_attr = readers[0];

    std::cout << "readers[0].node_name: " << role_attr.node_name << std::endl;
    std::cout << "readers[0].id: " << role_attr.id << std::endl;
    std::cout << "readers[0].channel_id: " << role_attr.channel_id << std::endl;
    std::cout << "readers[0].channel_name: " << role_attr.channel_name << std::endl;

    readers.clear();

    // reader离开后
    test.channel_manager_.Leave(role_attr , RoleType::ROLE_READER);
    //readers的大小就应该是0
    test.channel_manager_.GetReadersOfChannel("channel_0" , &readers);
    std::cout << "readers size: " << readers.size() << std::endl; 

    // reader重新加入后
    test.channel_manager_.Join(role_attr , RoleType::ROLE_READER);
    //readers的大小就应该是1
    test.channel_manager_.GetReadersOfChannel("channel_0" , &readers);
    std::cout << "readers size: " << readers.size() << std::endl; 

    role_attr.host_name = GlobalData::Instance()->HostName();
    role_attr.process_id =  GlobalData::Instance()->ProcessId();
    role_attr.node_name = "node_extra";
    role_attr.node_id = GlobalData::RegisterNode("node_extra");

    // 添加100个readers 都是指向node_extra的
    for (int i = 0; i < 100; ++i) {
        role_attr.channel_name = "channel_" + std::to_string(i);
        role_attr.channel_id = GlobalData::Instance()->RegisterChannel(role_attr.channel_name);
        hnu::cmw::transport::Identity id;
        role_attr.id = id.HashValue();
        test.channel_manager_.Join(role_attr, RoleType::ROLE_READER);
    }

    readers.clear();
    test.channel_manager_.GetReadersOfNode("node_extra", &readers);
    //此时readers的大小就应该是100
    std::cout << "readers size: " << readers.size() << std::endl; 
}


void TEST_CHANGE()
{
    ChannelManagerTest test;
    RoleAttributes role_attr;
    test.channel_manager_.Join(role_attr, RoleType::ROLE_NODE);
    test.channel_manager_.Join(role_attr, RoleType::ROLE_WRITER);

    role_attr.host_name = GlobalData::Instance()->HostName();
    role_attr.process_id =  GlobalData::Instance()->ProcessId();
    role_attr.node_name = "add_change";
    role_attr.node_id = GlobalData::RegisterNode("add_change");
    role_attr.channel_name = "wasd";
    role_attr.channel_id = GlobalData::RegisterChannel("wasd");

    hnu::cmw::transport::Identity id;
    role_attr.id = id.HashValue();
    test.channel_manager_.Join(role_attr , RoleType::ROLE_WRITER);
    
    std::cout<< boolalpha;
    std::cout << "HasWriter channel_0: " << test.channel_manager_.HasWriter("channel_0") << std::endl; 

}
int main()
{
    //TEST_JOIN();
    TEST_GET_CHANNEL_NAMES();
    TEST_HAS_WRITER_AND_READER();
    TEST_GET_WRITERS_ATTR();
    TEST_GET_READERS_ATTR();
    TEST_CHANGE();
    return 0;
}
