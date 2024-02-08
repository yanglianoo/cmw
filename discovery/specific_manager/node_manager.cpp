#include <cmw/discovery/specific_manager/node_manager.h>
#include <cmw/common/log.h>
#include <cmw/common/global_data.h>

namespace hnu {
namespace cmw {
namespace discovery{ 

NodeManager::NodeManager() {
  allowed_role_ |= 1 << RoleType::ROLE_NODE;
  change_type_ = ChangeType::CHANGE_NODE;
  channel_name_ = "node_change_broadcast";
}


NodeManager::~NodeManager() {}

bool NodeManager::Check(const RoleAttributes& attr){
    RETURN_VAL_IF(attr.node_name.empty(),false);
    RETURN_VAL_IF(!attr.node_id,false);
    return true;
}

bool NodeManager::HasNode(const std::string& node_name){
    //所有的Node都会注册到全局的map中
    uint64_t key = common::GlobalData::RegisterNode(node_name);
    return nodes_.Search(key);
}

void NodeManager::GetNodes(RoleAttrVec* nodes){
    RETURN_IF_NULL(nodes);
    nodes_.GetAllRoles(nodes);
}


void NodeManager::Dispose(const ChangeMsg& msg){
    if(msg.operate_type == OperateType::OPT_JOIN){
        DisposeJoin(msg);
    }else{
        DisposeLeave(msg);
    }
    //通知
    Notify(msg);
}


//拓扑机制结束
void NodeManager::OnTopoModuleLeave(const std::string& host_name,
                                    int process_id) {

  RETURN_IF(!is_discovery_started_.load());

  RoleAttributes attr;
  attr.host_name = host_name;
  attr.process_id = process_id;
  std::vector<RolePtr> nodes_to_remove;
  nodes_.Search(attr, &nodes_to_remove);
  for (auto& node : nodes_to_remove) {
    nodes_.Remove(node->attributes().node_id);
  }

  ChangeMsg msg;
  for (auto& node : nodes_to_remove) {
    Convert(node->attributes(), RoleType::ROLE_NODE, OperateType::OPT_LEAVE,
            &msg);
    Notify(msg);
  }
}

void NodeManager::DisposeJoin(const ChangeMsg& msg){

    auto node = std::make_shared<RoleNode>(msg.role_attr , msg.timestamp);
    uint64_t key = node->attributes().node_id;

    if(!nodes_.Add(key, node ,false)){
        //如果已经存在
        RolePtr existing_node;
        
        if (!nodes_.Search(key, &existing_node)) {
            //如果没找到，则覆盖
            nodes_.Add(key, node);
            return;
        }

        //到这里说明到了一个已经存在的existing_node
        RolePtr newer_node = existing_node;
        //根据时间判断两个node的先后关系
        if (node->IsEarlierThan(*newer_node)) {
            //如果node 比 newer_node早，则用node覆盖
            nodes_.Add(key, node);
        } else {
            //如果node比 newer_node晚，则将existing_node置为node
            newer_node = node;
        }

        //不允许添加相同的node
        if (newer_node->attributes().process_id == process_id_ &&
            newer_node->attributes().host_name == host_name_) {
            std::cout << "this process will be terminated due to duplicated node["
             << node->attributes().node_name
             << "], please ensure that each node has a unique name."<<std::endl;
            //AsyncShutdown();
        }
    }
}

void NodeManager::DisposeLeave(const ChangeMsg& msg){
    auto node = std::make_shared<RoleNode>(msg.role_attr , msg.timestamp);
    nodes_.Remove(node->attributes().node_id);
}

}
}
}