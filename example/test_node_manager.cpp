#include <cmw/discovery/specific_manager/node_manager.h>
#include <memory>
#include <cmw/config/RoleAttributes.h>
#include <cmw/common/global_data.h>
using namespace hnu::cmw::discovery;
using namespace hnu::cmw::config;
using namespace hnu::cmw::common;

void TEST_NODE_CHANGE(){
    auto node_manager_ = std::make_shared<NodeManager>();
    std::cout<< boolalpha;
    std::cout << "HasNode test 1: " << node_manager_->HasNode("node") << std::endl;

    RoleAttributes role_attr;
    role_attr.host_name = GlobalData::Instance()->HostName();
    role_attr.process_id =  GlobalData::Instance()->ProcessId();
    role_attr.node_name = "node";
    role_attr.node_id = GlobalData::RegisterNode("node");

    std::cout << "Join test 1: " << node_manager_->Join(role_attr,RoleType::ROLE_NODE)<< std::endl;    //Node类型的允许
    std::cout << "Join test 2: " << node_manager_->Join(role_attr,RoleType::ROLE_WRITER)<< std::endl;  //WRITER类型的不允许

    std::cout << "HasNode test 2: " << node_manager_->HasNode("node") << std::endl;

    std::cout << "Leave test 1: " << node_manager_->Leave(role_attr,RoleType::ROLE_NODE)<< std::endl;    //Node类型的允许
    std::cout << "Leave test 2: " << node_manager_->Leave(role_attr,RoleType::ROLE_WRITER)<< std::endl;  //WRITER类型的不允许

    std::cout << "HasNode test 3: " << node_manager_->HasNode("node") << std::endl;
}

void TEST_LISTENER(){
    
}

int main()
{
    TEST_NODE_CHANGE();
    return 0;
}



