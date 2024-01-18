#ifndef CMW_DISCOVERY_SPECIFIC_MANAGER_CHANNEL_MANAGER_H_
#define CMW_DISCOVERY_SPECIFIC_MANAGER_CHANNEL_MANAGER_H_

#include <unordered_set>
#include <vector>

#include <cmw/discovery/container/graph.h>
#include <cmw/discovery/container/multi_value_warehouse.h>
#include <cmw/discovery/container/single_value_warehouse.h>
#include <cmw/discovery/role/role.h>
#include <cmw/discovery/specific_manager/manager.h>

namespace hnu {
namespace cmw {
namespace discovery{ 


class TopologyManager;

class ChannelManager : public Manager {

public:
    using RoleAttrVec = std::vector<RoleAttributes>;
    using WriterWarehouse = MultiValueWarehouse;
    using ReaderWarehouse = MultiValueWarehouse;
    using ExemptedMessageTypes = std::unordered_set<std::string>;


private:

    bool Check(const RoleAttributes& attr) override;
    void Dispose(const ChangeMsg& msg) override;
    void OnTopoModuleLeave(const std::string& host_name, int process_id) override;

    void DisposeJoin(const ChangeMsg& msg);
    void DisposeLeave(const ChangeMsg& msg);

    void ScanMessageType(const ChangeMsg& msg);
    
    Graph node_graph_;
    // key: node_id
    WriterWarehouse node_writers_;
    ReaderWarehouse node_readers_;
    // key: channel_id
    WriterWarehouse channel_writers_;
    ReaderWarehouse channel_readers_;
};

}
}
}

#endif