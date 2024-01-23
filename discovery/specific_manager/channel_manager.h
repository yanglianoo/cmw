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


    ChannelManager();
    virtual ~ChannelManager();
 
    
    void GetWritersOfChannel(const std::string& channel_name,
                             RoleAttrVec* writers);
    void GetReadersOfChannel(const std::string& channel_name,
                             RoleAttrVec* readers);
    void GetWritersOfNode(const std::string& node_name, RoleAttrVec* writers);      
    void GetReadersOfNode(const std::string& node_name, RoleAttrVec* readers);

      /**
   * @brief Get the Upstream Of Node object.
   * If Node A has writer that publishes channel-1, and Node B has reader that
   * subscribes channel-1 then A is B's Upstream node, and B is A's Downstream
   * node
   *
   * @param node_name node's name we want to inquire
   * @param upstream_nodes result RoleAttribute vector
   */
    void GetUpstreamOfNode(const std::string& node_name,
                            RoleAttrVec* upstream_nodes);
        /**
   * @brief Get the Downstream Of Node object.
   * If Node A has writer that publishes channel-1, and Node B has reader that
   * subscribes channel-1 then A is B's Upstream node, and B is A's Downstream
   * node
   *
   * @param node_name node's name we want to inquire
   * @param downstream_nodes result RoleAttribute vector
   */
    void GetDownstreamOfNode(const std::string& node_name,
                            RoleAttrVec* downstream_nodes);     

      /**
   * @brief Get the Flow Direction from `lhs_node_node` to `rhs_node_name`
   * You can see FlowDirection's description for more information
   * @return FlowDirection result direction
   */
    FlowDirection GetFlowDirection(const std::string& lhs_node_name,
                                    const std::string& rhs_node_name);      
    
                                            
    void GetChannelNames(std::vector<std::string>* channels);
    void GetMsgType(const std::string& channel_name, std::string* msg_type);

    bool HasWriter(const std::string& channel_name);
    void GetWriters(RoleAttrVec* writers);
    bool HasReader(const std::string& channel_name);
    void GetReaders(RoleAttrVec* readers);


    bool IsMessageTypeMatching(const std::string& lhs, const std::string& rhs);
    
private:

    bool Check(const RoleAttributes& attr) override;
    //处理ChangeMsg
    void Dispose(const ChangeMsg& msg) override;
    void OnTopoModuleLeave(const std::string& host_name, int process_id) override;

    
    void DisposeJoin(const ChangeMsg& msg);
    void DisposeLeave(const ChangeMsg& msg);


    void ScanMessageType(const ChangeMsg& msg);
    

    ExemptedMessageTypes exempted_msg_types_;

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