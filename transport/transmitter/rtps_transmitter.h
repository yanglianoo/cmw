#ifndef CMW_TRANSPORT_TRANSMITTER_RTPS_TRANSMITTER_H_
#define CMW_TRANSPORT_TRANSMITTER_RTPS_TRANSMITTER_H_

#include <memory>
#include <string>
#include <cmw/transport/transmitter/transmitter.h>
#include <cmw/config/RoleAttributes.h>
#include <cmw/transport/rtps/participant.h>
#include <cmw/transport/rtps/attributes_filler.h>
#include <fastrtps/rtps/RTPSDomain.h>
using namespace eprosima::fastrtps::rtps;
using namespace eprosima::fastrtps;


namespace hnu    {
namespace cmw   {
namespace transport {

using namespace config;

template <typename M>
class RtpsTransmitter : public Transmitter<M> {

public:
    using MessagePtr = std::shared_ptr<M>;

    RtpsTransmitter(const RoleAttributes& attr,
                    const ParticipantPtr& participant);

    virtual ~RtpsTransmitter();

    void Enable() override;
    void Disable() override;

    bool Transmit(const MessagePtr& msg, const MessageInfo& msg_info) override;

private:
    bool Transmit(const M& msg, const MessageInfo& msg_info);

    ParticipantPtr participant_;


    eprosima::fastrtps::rtps::RTPSWriter* rtps_writer;
    eprosima::fastrtps::rtps::WriterHistory* mp_history;

};


template <typename M>
RtpsTransmitter<M>::RtpsTransmitter(const RoleAttributes& attr,
                                    const ParticipantPtr& participant)
        :Transmitter<M>(attr) , participant_(participant) , rtps_writer(nullptr) {}


template <typename M>
RtpsTransmitter<M>::~RtpsTransmitter() {
  Disable();
}

template <typename M>
void RtpsTransmitter<M>::Enable(){

    if(this->enabled_){
        return;
    }

    RtpsWriterAttributes writer_attr;


    AttributesFiller::FillInWriterAttr(
        this->attr_.channle_name, this->attr_.qos_profile,&writer_attr);
    
    //创建rtps writer history
    mp_history = new WriterHistory(writer_attr.hatt);

    //创建rtps writer
    rtps_writer  = RTPSDomain::createRTPSWriter(participant_->fastrtps_participant(), writer_attr.watt , mp_history);
    //注册rtps writer
    bool reg = participant_->fastrtps_participant()->registerWriter(rtps_writer , writer_attr.Tatt , writer_attr.Wqos);

    if(reg)
    {
      this->enabled_ = true;
    }

}

template <typename M>
void RtpsTransmitter<M>::Disable() {
  if (this->enabled_) {
    rtps_writer = nullptr;
    this->enabled_ = false;
  }
}


template <typename M>
bool RtpsTransmitter<M>::Transmit(const MessagePtr& msg,
                                  const MessageInfo& msg_info) {
  return Transmit(*msg, msg_info);
}

template <typename M>
bool RtpsTransmitter<M>::Transmit(const M& msg, const MessageInfo& msg_info) {
  if (!this->enabled_) {
    std::cout << "not enable." << std::endl;
    return false;
  }
  
  //发送数据
  CacheChange_t* ch = rtps_writer->new_change([]() -> uint32_t
                        {
                          return 255;
                        }, ALIVE);

  eprosima::fastrtps::rtps::WriteParams wparams;
  char* ptr = reinterpret_cast<char*>(&wparams.related_sample_identity().writer_guid());
  memcpy(ptr, msg_info.sender_id().data() , ID_SIZE);
  memcpy(ptr + ID_SIZE , msg_info.spare_id().data() ,ID_SIZE);

  //sequenceNumber 
  wparams.related_sample_identity().sequence_number().high = (int32_t)((msg_info.seq_num() & 0xFFFFFFFF00000000) >> 32);
  wparams.related_sample_identity().sequence_number().low = (int32_t)(msg_info.seq_num() & 0xFFFFFFFF);


  //数据装载 暂未实现
  // ch->serializedPayload.length = 
  // ch->

  //发送数据
  mp_history->add_change(ch,wparams);

  if(participant_->is_shutdown())
  {
    return false;
  }

}


}
}
}




#endif