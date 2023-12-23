
#include <cmw/discovery/specific_manager/manager.h>
#include <cmw/common/global_data.h>
#include <cmw/transport/rtps/attributes_filler.h>
#include <fastrtps/rtps/RTPSDomain.h>
#include <cmw/transport/rtps/attributes_filler.h>
#include <cmw/transport/qos/qos_profile_conf.h>
#include <cmw/transport/rtps/participant.h>
#include <fastrtps/rtps/reader/RTPSReader.h>
namespace hnu {
namespace cmw {
namespace discovery{ 

using namespace transport;
Manager::Manager()
    : is_shutdown_(false),
      is_discovery_started_(false),
      allowed_role_(0),
      change_type_(ChangeType::CHANGE_PARTICIPANT),
      channel_name_(""),
      writer_(nullptr),
      reader_(nullptr),
      listener_(nullptr) {
  host_name_ = common::GlobalData::Instance()->HostName();
  process_id_ = common::GlobalData::Instance()->ProcessId();
}

Manager::~Manager() { Shutdown(); }

//开启服务发现机制
bool Manager::StartDiscovery(RtpsParticipant* participant){
    if (participant == nullptr) {
        return false;
    }
    if (is_discovery_started_.exchange(true)) {
        return true;
    }
    if(!CreateWriter(participant) || !CreateReader(participant)){
        std::cout << "create writer or reader failed." << std::endl;
        StopDiscovery();
        return false;
    }
    return true;
}

void Manager::StopDiscovery(){
    if (!is_discovery_started_.exchange(false)) {
        return;
    }
    //为什么移除writer需要加锁
    {
        std::lock_guard<std::mutex> lg(lock_);
        if(writer_ != nullptr){
            eprosima::fastrtps::rtps::RTPSDomain::removeRTPSWriter(writer_);
        }
    }

    if(reader_ != nullptr){
        eprosima::fastrtps::rtps::RTPSDomain::removeRTPSReader(reader_);
    }

    if(listener_ != nullptr){
        delete listener_;
        listener_ = nullptr;
    }
}

void Manager::Shutdown(){
    if(is_shutdown_.exchange(true)){
        return;
    }
    StopDiscovery();
    //信号槽取消对槽函数的绑定
    signal_.DisconnectAllSlots();
}

bool Manager::Join(const RoleAttributes& attr, RoleType role,
                  bool need_write = true){
    if(is_shutdown_.load()){
        std::cout << "the manager has been shut down." << std::endl;
        return false;
    }          

}

bool Manager::Leave(const RoleAttributes& attr, RoleType role){
    if(is_shutdown_.load()){
        std::cout << "the manager has been shut down." << std::endl;
        return false;
    }   
}

Manager::ChangeConnection Manager::AddChangeListener(const ChangeFunc& func){
   return  signal_.Connect(func);
}

void Manager::RemoveChangeListener(const ChangeConnection& conn) {
  auto local_conn = conn;
  //信号内部会删除此槽函数
  local_conn.Disconnect();
}

bool Manager::CreateReader(RtpsParticipant* participant){

    RtpsReaderAttributes reader_attr;

    AttributesFiller::FillInReaderAttr(
                channel_name_ , QosProfileConf::QOS_PROFILE_TOPO_CHANGE , &reader_attr);

    listener_ = new ReaderListener(
            std::bind(&Manager::OnRemoteChange, this , std::placeholders::_1));
    eprosima::fastrtps::rtps::ReaderHistory* mp_history = new ReaderHistory(reader_attr.hatt);

    reader_ = RTPSDomain::createRTPSReader(participant , reader_attr.ratt , mp_history ,listener_);

    bool reg = participant->registerReader(reader_ , reader_attr.Tatt , reader_attr.Rqos);
    return reg;
}

bool Manager::CreateWriter(RtpsParticipant* participant){
    // 创建 RtpsWriter 的配置信息实例
    RtpsWriterAttributes writer_attr; 
    // 填充 RtpsWriter 的配置信息
    AttributesFiller::FillInWriterAttr(
        channel_name_, QosProfileConf::QOS_PROFILE_TOPO_CHANGE,&writer_attr);
    
    eprosima::fastrtps::rtps::WriterHistory* mp_history = new WriterHistory(writer_attr.hatt);

    writer_ = RTPSDomain::createRTPSWriter(participant , writer_attr.watt , mp_history);

    bool reg = participant->registerWriter(writer_ , writer_attr.Tatt , writer_attr.Wqos);

    return reg;
}

void Manager::OnRemoteChange(const std::string& str_msg){
    if(is_shutdown_.load()){
        std::cout <<  "the manager has been shut down." << std::endl;
        return;
    }
    ChangeMsg msg;



}

bool Manager::IsFromSameProcess(const ChangeMsg& msg) {
    auto& host_name = msg.role_attr.host_name;
    int process_id = msg.role_attr.process_id;
    if (process_id != process_id_ || host_name != host_name_) {
        return false;
    }
  return true;
}

}
}
}