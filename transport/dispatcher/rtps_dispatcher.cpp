#include <cmw/transport/dispatcher/rtps_dispatcher.h>
#include <cmw/transport/rtps/attributes_filler.h>
#include <fastrtps/rtps/RTPSDomain.h>
#include <fastrtps/rtps/reader/RTPSReader.h>
namespace hnu    {
namespace cmw   {
namespace transport {

RtpsDispatcher::RtpsDispatcher() : participant_(nullptr) {}
RtpsDispatcher::~RtpsDispatcher() { Shutdown(); }

void RtpsDispatcher::Shutdown() {
  if (is_shutdown_.exchange(true)) {
    return;
  }

  {
    std::lock_guard<std::mutex> lock(readers_mutex_);
    for (auto& item : readers_) {
      item.second.reader = nullptr;
    }
  }

  participant_ = nullptr;
}

void RtpsDispatcher::AddReader(const RoleAttributes& self_attr) {
    if (participant_ == nullptr) {
    std::cout << "please set participant firstly." << std::endl;
    return;
  }

    uint64_t channel_id = self_attr.channel_id;
    std::lock_guard<std::mutex> lock(readers_mutex_);

    if(readers_.count(channel_id) > 0){
        return;
    }

    //创建一个reader
    Reader new_reader;
    //填充reader的配置信息
    RtpsReaderAttributes reader_attr;

    auto& qos = self_attr.qos_profile;
    AttributesFiller::FillInReaderAttr(self_attr.channel_name , qos , &reader_attr);
    //创建reader的回调函数
    new_reader.reader_listener = std::make_shared<ReaListener>(
        std::bind(&RtpsDispatcher::OnMessage, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3),self_attr.channel_name);
    //创建rtps reader history
    new_reader.mp_history = new ReaderHistory(reader_attr.hatt);
    //创建rtps reader
    new_reader.reader = RTPSDomain::createRTPSReader(
                    participant_->fastrtps_participant(),reader_attr.ratt , new_reader.mp_history,
                    new_reader.reader_listener.get());
    //注册rtps reader
    bool reg =  participant_->fastrtps_participant()->registerReader(new_reader.reader,reader_attr.Tatt,reader_attr.Rqos);

    readers_[channel_id] = new_reader;          

}

void RtpsDispatcher::OnMessage(uint64_t channel_id,
                               const std::shared_ptr<std::string>& msg_str,
                               const MessageInfo& msg_info) {
    if (is_shutdown_.load()) {
    return;
  }     

  ListenerHandlerBasePtr* handler_base = nullptr;
  if (msg_listeners_.Get(channel_id, &handler_base)) {
    auto handler =
        std::dynamic_pointer_cast<ListenerHandler<std::string>>(*handler_base);
    //根据信号执行注册的槽函数
    handler->Run(msg_str, msg_info);
  }
}

}
}
}