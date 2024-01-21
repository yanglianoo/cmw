
#include <cmw/discovery/specific_manager/manager.h>
#include <cmw/common/global_data.h>
#include <cmw/transport/rtps/attributes_filler.h>
#include <fastrtps/rtps/RTPSDomain.h>
#include <cmw/transport/rtps/attributes_filler.h>
#include <cmw/transport/qos/qos_profile_conf.h>
#include <cmw/transport/rtps/participant.h>
#include <fastrtps/rtps/reader/RTPSReader.h>
#include <cmw/base/macros.h>
#include <cmw/time/time.h>
#include <cmw/serialize/data_stream.h>

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
    //将is_discovery_started_标志位设置为true
    if (is_discovery_started_.exchange(true)) {
        return true;
    }

    //创建 rtpsWriter 和 rtspReader
    if(!CreateWriter(participant) || !CreateReader(participant)){
        std::cout << "create writer or reader failed." << std::endl;
        StopDiscovery();
        return false;
    }
    return true;
}

//终止服务发现机制
void Manager::StopDiscovery(){
    if (!is_discovery_started_.exchange(false)) {
        return;
    }
    //为什么移除writer需要加锁
    {
        std::lock_guard<std::mutex> lg(lock_);
        if(writer_ != nullptr){
            //从rtpsRTPSDomain 中移除writer
            eprosima::fastrtps::rtps::RTPSDomain::removeRTPSWriter(writer_);
        }
    }

    if(reader_ != nullptr){
        //从rtpsRTPSDomain 中移除reader
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


/*加入拓扑网络*/
bool Manager::Join(const RoleAttributes& attr, RoleType role,
                  bool need_write ){
    if(is_shutdown_.load()){
        std::cout << "the manager has been shut down." << std::endl;
        return false;
    }    

    //
    RETURN_VAL_IF(!((1 << role) & allowed_role_), false);
    //创建一个msg
    ChangeMsg msg;
    //填充msg
    Convert(attr, role, OperateType::OPT_JOIN, &msg);
    //
    Dispose(msg);

    //广播msg
    if (need_write) {
        return Write(msg);
    }
    return true;


}

/*离开拓扑网络*/
bool Manager::Leave(const RoleAttributes& attr, RoleType role){
    if(is_shutdown_.load()){
        std::cout << "the manager has been shut down." << std::endl;
        return false;
    }   
    RETURN_VAL_IF(!((1 << role) & allowed_role_), false);
    RETURN_VAL_IF(!Check(attr), false);
    ChangeMsg msg;
    Convert(attr, role, OperateType::OPT_LEAVE, &msg);
    Dispose(msg);
    if (NeedPublish(msg)) {
        return Write(msg);
    }
  return true;
}

//添加回调函数，绑定信号槽
Manager::ChangeConnection Manager::AddChangeListener(const ChangeFunc& func){
   return  signal_.Connect(func);
}

//移除回调函数，
void Manager::RemoveChangeListener(const ChangeConnection& conn) {
  auto local_conn = conn;
  //信号内部会删除此槽函数
  local_conn.Disconnect();
}

//创建rtspReader
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

//创建rtpsWriter
bool Manager::CreateWriter(RtpsParticipant* participant){
    // 创建 RtpsWriter 的配置信息实例
    RtpsWriterAttributes writer_attr; 
    // 填充 RtpsWriter 的配置信息
    AttributesFiller::FillInWriterAttr(
        channel_name_, QosProfileConf::QOS_PROFILE_TOPO_CHANGE,&writer_attr);
    
    //创建rtps writer history
    writer_history_ = new WriterHistory(writer_attr.hatt);
    //创建rtps writer
    writer_ = RTPSDomain::createRTPSWriter(participant , writer_attr.watt , writer_history_);
    //注册rtps writer
    bool reg = participant->registerWriter(writer_ , writer_attr.Tatt , writer_attr.Wqos);

    return reg;
}

bool Manager::NeedPublish(const ChangeMsg& msg) const {
  (void)msg;
  return true;
}


void Manager::OnRemoteChange(const std::string& str_msg){
    if(is_shutdown_.load()){
        std::cout <<  "the manager has been shut down." << std::endl;
        return;
    }


    ChangeMsg msg;

    serialize::DataStream ds(str_msg);
    //需要将str_msg 反序列化成ChangeMsg类型的msg
    ds >> msg;

    //判断是否是同一进程
    if(IsFromSameProcess(msg)){
        std::cout << "FromSameProcess" << std::endl;
        return;
    }


    RETURN_IF(!Check(msg.role_attr));

    Dispose(msg);
    

}


//填充msg
void Manager::Convert(const RoleAttributes& attr, RoleType role, OperateType opt,
               ChangeMsg* msg){
    
    msg->timestamp = cmw::Time::Now().ToNanosecond();  //时间戳为ns
    msg->change_type = change_type_;
    msg->operate_type = opt;
    msg->role_type = role;

    msg->role_attr = attr;

    if(msg->role_attr.host_name.empty()){
        msg->role_attr.host_name = host_name_;
    }
    if(!msg->role_attr.process_id){
        msg->role_attr.process_id = process_id_;
    }       
}

//槽函数通知执行回调
void Manager::Notify(const ChangeMsg& msg) { signal_(msg); }

//判断是否是同一进程
bool Manager::IsFromSameProcess(const ChangeMsg& msg){
    auto& host_name = msg.role_attr.host_name;
    int process_id = msg.role_attr.process_id;

    if (process_id != process_id_ || host_name != host_name_) {
        return false;
    }
    return true;
}


bool Manager::Write(const ChangeMsg& msg){
//使用eprosima::fastrtps::rtps::RTPSWriter* writer_ 发布数据
  
  //判断discovery是否启动了
  if(!is_discovery_started_.load()){
    std::cout << "discovery is not started." << std::endl;
    return false;
  }

  //将ChangeMsg序列化
  serialize::DataStream ds;
  ds << msg;
  
  {
    //
    std::lock_guard<std::mutex> lg(lock_);
    if(writer_ != nullptr){
    //发送数据
    CacheChange_t* ch = writer_->new_change([]() -> uint32_t
                        {
                          return 255;
                        }, ALIVE);
    //数据装载 
    ch->serializedPayload.length = ds.size();
    std::memcpy((char*)ch->serializedPayload.data , ds.data(), ds.size());
    
    bool flag = writer_history_->add_change(ch);
    if(!flag)
    {
        writer_->remove_older_changes(20);
        writer_history_->add_change(ch);
    }
    }
  }
  return true;
}


}
}
}