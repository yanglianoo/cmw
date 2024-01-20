#ifndef CMW_TRANSPORT_DISPATCHER_RTPS_DISPATCHER_H_
#define CMW_TRANSPORT_DISPATCHER_RTPS_DISPATCHER_H_


#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <cmw/common/macros.h>
#include <cmw/transport/dispatcher/dispatcher.h>
#include <cmw/transport/rtps/attributes_filler.h>
#include <cmw/transport/rtps/participant.h>
#include <cmw/transport/rtps/rea_listener.h>
#include <fastrtps/rtps/rtps_fwd.h>
#include <cmw/serialize/data_stream.h>

namespace hnu    {
namespace cmw   {
namespace transport {

struct Reader {
    Reader() : reader(nullptr) , reader_listener(nullptr) {}

    eprosima::fastrtps::rtps::RTPSReader* reader;
    eprosima::fastrtps::rtps::ReaderHistory* mp_history;
    RealistenerPtr reader_listener;
};

class RtpsDispatcher;
using RtpsDispatcherPtr = RtpsDispatcher*;

class RtpsDispatcher : public Dispatcher {

public:
    virtual ~RtpsDispatcher();

    void Shutdown() override;

    template <typename MessageT>
    void AddListener(const RoleAttributes& self_attr,
                     const MessageListener<MessageT>& listener);

    template <typename MessageT>
    void AddListener(const RoleAttributes& self_attr,
                     const RoleAttributes& opposite_attr,
                     const MessageListener<MessageT>& listener);

    void set_participant(const ParticipantPtr& participant) {
        participant_ = participant;
  }
private:
    void OnMessage(uint64_t channel_id,
                   const std::shared_ptr<std::string>& msg_str,
                   const MessageInfo& msg_info);
    void AddReader(const RoleAttributes& self_attr);
    std::unordered_map<uint64_t , Reader> readers_;
    std::mutex readers_mutex_;
    ParticipantPtr participant_;

    DECLARE_SINGLETON(RtpsDispatcher)

};


template <typename MessageT>
void RtpsDispatcher::AddListener(const RoleAttributes& self_attr,
                                 const MessageListener<MessageT>& listener) {

    auto listener_adapter = [listener](const std::shared_ptr<std::string>& msg_str, 
                                       const MessageInfo& msg_info){
            auto msg = std::make_shared<MessageT>();
            serialize::DataStream ds(*msg_str);
            ds >> *msg;
            listener(msg , msg_info);
    };
    //调用基类的AddListener来注册回调函数
    Dispatcher::AddListener<std::string>(self_attr ,listener_adapter);
    AddReader(self_attr);

}

template <typename MessageT>
void RtpsDispatcher::AddListener(const RoleAttributes& self_attr,
                                 const RoleAttributes& opposite_attr,
                                 const MessageListener<MessageT>& listener){

    auto listener_adapter = [listener](const std::shared_ptr<std::string>& msg_str, 
                                       const MessageInfo& msg_info){
            auto msg = std::make_shared<MessageT>();
            serialize::DataStream ds(*msg_str);
            ds >> *msg;
            listener(msg , msg_info);
    };
    //调用基类的AddListener来注册回调函数
    Dispatcher::AddListener<std::string>(self_attr,opposite_attr,listener_adapter);
    //创建一个rtps reader
    AddReader(self_attr);
}


}
}
}



#endif