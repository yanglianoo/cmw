#include <cmw/discovery/topology_manager.h>
#include <cmw/common/global_data.h>
#include <fastdds/rtps/participant/ParticipantDiscoveryInfo.h>
#include <cmw/time/time.h>
namespace hnu {
namespace cmw {
namespace discovery{ 

using namespace eprosima::fastrtps::rtps;
TopologyManager::TopologyManager()
    : init_(false),
      node_manager_(nullptr),
      participant_(nullptr),
      participant_listener_(nullptr){
    
    Init();

}


bool TopologyManager::Init(){
    if (init_.exchange(true)) {
        return true;
    }

    node_manager_ = std::make_shared<NodeManager>();

    CreateParticipant();

    bool result = InitNodeManager();

    if (!result) {
        std::cout << "init manager failed."<<std::endl;
        participant_ = nullptr;
        delete participant_listener_;
        participant_listener_ = nullptr;
        node_manager_ = nullptr;
        init_.store(false);
        return false;
    }

    return true;
}

bool TopologyManager::InitNodeManager() {
  return node_manager_->StartDiscovery(participant_->fastrtps_participant());
}

bool TopologyManager::CreateParticipant(){
    std::string participant_name =
      common::GlobalData::Instance()->HostName() + '+' +
      std::to_string(common::GlobalData::Instance()->ProcessId());
    
    //创建RTPSParticipantListener
    participant_listener_ = new ParticipantListener(std::bind(
            &TopologyManager::OnParticipantChange, this , std::placeholders::_1));
    //创建RTPSParticipant
    participant_ = std::make_shared<transport::Participant>(
        participant_name, 11511 , participant_listener_);
    
    return true;
}

void TopologyManager::OnParticipantChange(const PartInfo& info){


}


bool TopologyManager::Convert(const PartInfo& info, ChangeMsg* change_msg){
    auto guid = info.info.m_guid;
    auto status = info.status;
    std::string participant_name("");
    OperateType opt_type = OperateType::OPT_JOIN;


    switch (status)
    {
        case ParticipantDiscoveryInfo::DISCOVERY_STATUS::DISCOVERED_PARTICIPANT:
            participant_name = info.info.m_participantName;
            participant_names_[guid] = participant_name;
            opt_type = OperateType::OPT_JOIN;
            break;
        
        case ParticipantDiscoveryInfo::DISCOVERY_STATUS::REMOVED_PARTICIPANT:

        case ParticipantDiscoveryInfo::DISCOVERY_STATUS::DROPPED_PARTICIPANT:
            if(participant_names_.find(guid) != participant_names_.end()){
                participant_name = participant_names_[guid];
                participant_names_.erase(guid);
            }
            opt_type = OperateType::OPT_LEAVE;
            break;
    
    default:
        break;
    }

    std::string host_name("");
    int process_id = 0;
    if(!ParseParticipantName(participant_name , &host_name , &process_id)){
        return false;
    }

    change_msg->timestamp = Time::Now().ToNanosecond();
    change_msg->change_type = ChangeType::CHANGE_PARTICIPANT;
    change_msg->operate_type = opt_type;
    change_msg->role_type = RoleType::ROLE_PARTICIPANT;

    // role attr 
    change_msg->role_attr.host_name = host_name;
    change_msg->role_attr.process_id = process_id;

    return true;
}


bool TopologyManager::ParseParticipantName(const std::string& participant_name,
                            std::string* host_name, int* process_id){
    
    auto pos = participant_name.find('+');
    if (pos == std::string::npos) {
        std::cout << "participant_name [" << participant_name << "] format mismatch."<< std::endl;
        return false;
    }
    *host_name = participant_name.substr(0 , pos);
    std::string pid_str = participant_name.substr(pos + 1);
    try {
        *process_id = std::stoi(pid_str);
    }catch (const std::exception& e){
        std::cout << "invalid process_id:" << e.what() << std::endl;
        return false;
    }
    return true;

}



}
}
}