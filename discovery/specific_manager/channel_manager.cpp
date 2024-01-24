#include <cmw/discovery/specific_manager/channel_manager.h>
#include <cmw/base/macros.h>
#include <cmw/common/global_data.h>
#include <set>

namespace hnu {
namespace cmw {
namespace discovery{ 


ChannelManager::ChannelManager() {
    allowed_role_ |= 1 << RoleType::ROLE_WRITER;
    allowed_role_ |= 1 << RoleType::ROLE_READER;
    change_type_ = ChangeType::CHANGE_CHANNEL;
    channel_name_ = "channel_change_broadcast";
}


ChannelManager::~ChannelManager() {}

bool ChannelManager::Check(const RoleAttributes& attr){
    RETURN_VAL_IF(attr.channel_name.empty(),false);
    RETURN_VAL_IF(!attr.channel_id, false);
    RETURN_VAL_IF(!attr.id, false);
    return true;
}


bool ChannelManager::IsMessageTypeMatching(const std::string& lhs, const std::string& rhs){
    if(lhs == rhs){
        return true;
    }


    return false;
}

//拿到所有的channel_name，保存到channels中
void ChannelManager::GetChannelNames(std::vector<std::string>* channels){
    RETURN_IF_NULL(channels);

    std::unordered_set<std::string>  local_channels;
    std::vector<RolePtr> roles;
    channel_writers_.GetAllRoles(&roles);
    channel_readers_.GetAllRoles(&roles);

    for(auto role : roles){
        local_channels.emplace(role->attributes().channel_name);
    }

    //转移所有权，尾插
    std::move(local_channels.begin() , local_channels.end() , std::back_inserter(*channels));

}


//获取MsgType
void ChannelManager::GetMsgType(const std::string& channel_name,
                                std::string* msg_type){

    RETURN_IF_NULL(msg_type);
    uint64_t key = common::GlobalData::RegisterChannel(channel_name);
    RolePtr writer = nullptr;
    if(!channel_writers_.Search(key , &writer)){
        std::cout << "cannot find writer of channel: " << channel_name
            << " key: " << key << std::endl;
             return;
    }

    if(!writer->attributes().message_type.empty())
    {
        *msg_type = writer->attributes().message_type;
    }
}

bool ChannelManager::HasWriter(const std::string& channel_name)
{
    uint64_t key = common::GlobalData::RegisterChannel(channel_name);
    return channel_writers_.Search(key);
}

void ChannelManager::GetWriters(RoleAttrVec* writers) {
  RETURN_IF_NULL(writers);
  channel_writers_.GetAllRoles(writers);
}

bool ChannelManager::HasReader(const std::string& channel_name) {
  uint64_t key = common::GlobalData::RegisterChannel(channel_name);
  return channel_readers_.Search(key);
}

void ChannelManager::GetReaders(RoleAttrVec* readers) {
  RETURN_IF_NULL(readers);
  channel_readers_.GetAllRoles(readers);
}


void ChannelManager::GetWritersOfChannel(const std::string& channel_name,
                                         RoleAttrVec* writers){
    RETURN_IF_NULL(writers);
    uint64_t key = common::GlobalData::RegisterChannel(channel_name);
    channel_writers_.Search(key , writers);
}


void ChannelManager::GetReadersOfChannel(const std::string& channel_name,
                                        RoleAttrVec* readers){
    RETURN_IF_NULL(readers);
    uint64_t key = common::GlobalData::RegisterChannel(channel_name);
    channel_readers_.Search(key , readers);
}

void ChannelManager::GetReadersOfNode(const std::string& node_name,
                                      RoleAttrVec* readers) {
  RETURN_IF_NULL(readers);
  uint64_t key = common::GlobalData::RegisterNode(node_name);
  node_readers_.Search(key, readers);
}

void ChannelManager::GetWritersOfNode(const std::string& node_name,
                                      RoleAttrVec* writers) {
  RETURN_IF_NULL(writers);
  uint64_t key = common::GlobalData::RegisterNode(node_name);
  node_writers_.Search(key, writers);
}

void ChannelManager::ScanMessageType(const ChangeMsg& msg){

    uint64_t key = msg.role_attr.channel_id;
    std::string role_type("reader");
    if(msg.role_type == RoleType::ROLE_WRITER){
        role_type = "writer";
    }

    RoleAttrVec existed_writers;
    channel_writers_.Search(key, &existed_writers);

    for(auto& w_attr : existed_writers){
        if(!IsMessageTypeMatching(w_attr.message_type , msg.role_attr.message_type)){
            std::cout << "newly added " << role_type << "(belongs to node["
             << msg.role_attr.node_name << "])"
             << "'s message type[" << msg.role_attr.message_type
             << "] does not match the exsited writer(belongs to node["
             << w_attr.node_name << "])'s message type["
             << w_attr.message_type << "]." << std::endl;
        }
    }

    RoleAttrVec existed_readers;

    channel_readers_.Search(key, &existed_readers);
    for(auto& r_attr : existed_readers){
        if (!IsMessageTypeMatching(msg.role_attr.message_type,
                               r_attr.message_type)) {
            std::cout << "newly added " << role_type << "(belongs to node["
             << msg.role_attr.node_name << "])"
             << "'s message type[" << msg.role_attr.message_type
             << "] does not match the exsited reader(belongs to node["
             << r_attr.node_name << "])'s message type["
             << r_attr.message_type << "]." << std::endl;
      }
    }
}


void ChannelManager::DisposeJoin(const ChangeMsg& msg){
    ScanMessageType(msg);

    //以Node作为顶点
    Vertice v(msg.role_attr.node_name);

    Edge e;
    //以channel_name作为边
    e.set_value(msg.role_attr.channel_name);

    //把writer作为出发顶点，reader作为目标顶点
    if(msg.role_type == RoleType::ROLE_WRITER){
        //
        
        auto role = std::make_shared<RoleWriter>(msg.role_attr, msg.timestamp);
        node_writers_.Add(role->attributes().node_id , role);
        channel_writers_.Add(role->attributes().channel_id, role);
        //设置此边的出发顶点
        e.set_src(v);
    } else {
        auto role = std::make_shared<RoleReader>(msg.role_attr , msg.timestamp);
        node_readers_.Add(role->attributes().node_id, role);
        channel_readers_.Add(role->attributes().channel_id, role);
        //设置此边的目标顶点
        e.set_dst(v);
    }
    //将此边加入图中
    node_graph_.Insert(e);
}

void ChannelManager::DisposeLeave(const ChangeMsg& msg){

    Vertice v(msg.role_attr.node_name);
    Edge e;
    e.set_value(msg.role_attr.channel_name);
    if(msg.role_type == RoleType::ROLE_WRITER){
        auto role = std::make_shared<RoleWriter>(msg.role_attr, msg.timestamp);
        node_writers_.Remove(role->attributes().node_id , role);
        channel_writers_.Remove(role->attributes().channel_id, role);
        e.set_src(v);
    } else {
        auto role = std::make_shared<RoleReader>(msg.role_attr , msg.timestamp);
        node_readers_.Remove(role->attributes().channel_id, role);
        channel_readers_.Remove(role->attributes().channel_id, role);
        //设置此边的目标顶点
        e.set_dst(v);
    }

    node_graph_.Delete(e);

}

void ChannelManager::Dispose(const ChangeMsg& msg){
    if(msg.operate_type == OperateType::OPT_JOIN){
        DisposeJoin(msg);
    } else {
        DisposeLeave(msg);
    }
    Notify(msg);
}


void ChannelManager::GetUpstreamOfNode(const std::string& node_name,
                                        RoleAttrVec* upstream_nodes){
    RETURN_IF_NULL(upstream_nodes);
    RoleAttrVec readers;
    //拿到node中所用的 reader
    GetReadersOfNode(node_name, &readers);
    if(readers.empty()){
        return;
    }

    //拿到各个reader对应的channel_name
    std::unordered_set<std::string> channels;
    for(auto& reader : readers){
        channels.emplace(reader.channel_name);
    }

    //根据reader对应的channel拿到指向的writer
    RoleAttrVec writers;
    for(auto& channel : channels)
    {
        GetWritersOfChannel(channel , &writers);
    }

    //拿到每个writer所属的node
    std::unordered_map<std::string , RoleAttributes> nodes;
    for(auto& writer : writers)
    {
        RoleAttributes attr;
        attr.host_name = writer.host_name;
        attr.process_id = writer.process_id;
        attr.node_name = writer.node_name;
        attr.node_id = writer.node_id;
        nodes[attr.node_name] = attr;
    }

    //将各个node保存到upstream_nodes中
    for(auto& item : nodes)
    {
        upstream_nodes->emplace_back(item.second);
    }
}

void ChannelManager::GetDownstreamOfNode(const std::string& node_name, 
                                         RoleAttrVec* downstream_nodes){
    RETURN_IF_NULL(downstream_nodes);

    //根据node_name拿到此node中所有的writers
    RoleAttrVec writers;
    GetWritersOfNode(node_name , &writers);
    if(writers.empty()){
        return;
    }

    //拿到各个writer对应的channel_name
    std::unordered_set<std::string> channels;
    for (auto& writer : writers){
        channels.emplace(writer.channel_name);
    }

    //拿到writer对应的channel指入的reader
    RoleAttrVec readers;
    for(auto& channel : channels)
    {
        GetReadersOfChannel(channel , &readers);
    }

    //拿到每个reader所属的node
    std::unordered_map<std::string, RoleAttributes> nodes;
    for(auto& reader : readers){
        RoleAttributes attr;
        attr.host_name = reader.host_name;
        attr.process_id = reader.process_id;
        attr.node_name = reader.node_name;
        attr.node_id = reader.node_id;
        nodes[attr.node_name] = attr;
    }

    //将node保存到downstream_nodes中
    for(auto& node : nodes)
    {
        downstream_nodes->emplace_back(node.second);
    }

}


FlowDirection ChannelManager::GetFlowDirection(const std::string& lhs_node_name,
                                     const std::string& rhs_node_name){
    Vertice lhs(lhs_node_name);
    Vertice rhs(rhs_node_name);
    return node_graph_.GetDirectionOf(lhs , rhs);        

}
void ChannelManager::OnTopoModuleLeave(const std::string& host_name,
                                       int process_id)
{

    RETURN_IF(!is_discovery_started_.load())

    RoleAttributes attr;
    attr.host_name = host_name;
    attr.process_id = process_id;

    std::vector<RolePtr> writers_to_remove;
    channel_writers_.Search(attr , &writers_to_remove);

    std::vector<RolePtr> readers_to_remove;
    channel_readers_.Search(attr, &readers_to_remove);

    ChangeMsg msg;
    for( auto& writer : writers_to_remove){
        Convert(writer->attributes(), RoleType::ROLE_WRITER, OperateType::OPT_LEAVE,
            &msg);
        DisposeLeave(msg);
        Notify(msg);
    }

    for (auto& reader : readers_to_remove) {
        Convert(reader->attributes(), RoleType::ROLE_READER, OperateType::OPT_LEAVE,
            &msg);
        DisposeLeave(msg);
        Notify(msg);
    }

}


}
}
}