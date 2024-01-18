#ifndef CMW_DISCOVERY_SPECIFIC_MANAGER_NODE_MANAGER_H_
#define CMW_DISCOVERY_SPECIFIC_MANAGER_NODE_MANAGER_H_


#include <memory>
#include <string>
#include <vector>
#include <cmw/discovery/specific_manager/manager.h>
#include <cmw/discovery/container/single_value_warehouse.h>
#include <cmw/discovery/role/role.h>

namespace hnu {
namespace cmw {
namespace discovery{ 


class NodeManager : public Manager{

public:
    using RoleAttrVec = std::vector<RoleAttributes>;
    using NodeWarehouse = SingleValueWarehouse;

    NodeManager();
    virtual ~NodeManager();

    /**
     * @brief Checkout whether we have `node_name` in topology
     *
     * @param node_name Node's name we want to inquire
     * @return true if this node found
     * @return false if this node not exits
     */
    bool HasNode(const std::string& node_name);

    /**
     * @brief Get the Nodes object
     *
     * @param nodes result RoleAttr vector
     */
    void GetNodes(RoleAttrVec* nodes);

private:

    bool Check(const RoleAttributes& attr) override;
    void Dispose(const ChangeMsg& msg) override;
    void OnTopoModuleLeave(const std::string& host_name,int process_id) override;

    void DisposeJoin(const ChangeMsg& msg);
    void DisposeLeave(const ChangeMsg& msg);

    NodeWarehouse nodes_;
};

}
}
}

#endif