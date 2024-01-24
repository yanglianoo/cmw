#include <cmw/discovery/topology_manager.h>

#include <cmw/config/RoleAttributes.h>

using namespace hnu::cmw::config;
using namespace hnu::cmw::discovery;
int main()
{   
    TopologyManager* topology_ = TopologyManager::Instance();

    auto conn = topology_->AddChangeListener([](const ChangeMsg& change_msg){
            if (change_msg.change_type == ChangeType::CHANGE_PARTICIPANT &&
            change_msg.operate_type == OperateType::OPT_JOIN &&
            change_msg.role_type == RoleType::ROLE_PARTICIPANT) {
            std::cout << change_msg.role_attr.host_name << std::endl;
            std::cout << change_msg.role_attr.process_id << std::endl;
        } 
    });

    while (1)
    {
       
    }
    
    return 0;
}
