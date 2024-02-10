#ifndef CMW_TRANSPORT_TRANSMITTER_SHM_TRANSMITTER_H_
#define CMW_TRANSPORT_TRANSMITTER_SHM_TRANSMITTER_H_

#include <cstring>
#include <iostream>
#include <memory>

#include <cmw/common/global_data.h>
#include <cmw/transport/shm/notifier_factory.h>
#include <cmw/transport/shm/readable_info.h>
#include <cmw/transport/shm/segment_factory.h>
#include <cmw/transport/transmitter/transmitter.h>
#include <cmw/common/util.h>
#include <cmw/common/log.h>
#include <cmw/serialize/data_stream.h>

namespace hnu    {
namespace cmw   {
namespace transport {

template <typename M>
class ShmTransmitter : public Transmitter<M> {

public:
    using MessagePtr = std:shared_ptr<M>;

    explicit ShmTransmitter(const RoleAttributes& attr);
    virtual ~ShmTransmitter();

    void Enable() override;
    void Disable() override;

    bool Transmit(const MessagePtr& msg, const MessageInfo& info) override;

private:
    bool Transmit(const M& msg, const MessageInfo& msg_info);

    SegmentPtr segment_;
    uint64_t channel_id_;
    uint64_t host_id_;
    NotifierPtr notifier_;
};

template <typename M>
ShmTransmitter<M>::ShmTransmitter(const RoleAttributes& attr)
    : Transmitter(attr),
      segment_(nullptr),
      channel_id_(attr.channel_id),
      notifier_(nullptr)
      {
        host_id_ = common::Hash(attr.host_ip);
}

template <typename M>
ShmTransmitter<M>::~ShmTransmitter(){
    Disable();
}

template <typename M>
void ShmTransmitter<M>::Enable(){
    if(this->enabled_){
        return;
    }

    segment_ = SegmentFactory::CreateSegment(channel_id_);
    notifier_ =NotifierFactory::CreateNotifier();
    this->enabled_ = true;
}

template <typename M>
void ShmTransmitter<M>::Disable(){
    if (this->enabled_) {
    segment_ = nullptr;
    notifier_ = nullptr;
    this->enabled_ = false;
  }
}

template <typename M>
bool ShmTransmitter<M>::Transmit(const MessagePtr& msg,
                                 const MessageInfo& msg_info) {
  return Transmit(*msg, msg_info);
}

template <typename M>
bool ShmTransmitter<M>::Transmit(const M& msg, const MessageInfo& msg_info){
    if (!this->enabled_) {
    ADEBUG << "not enable.";
    return false;
   }

    WritableBlock wb;
    //序列化成字符串
    serialize::DataStream ds; 
    ds << msg;
    //拿到序列化数据所占内存字节数
    std::size_t msg_size = ds.ByteSize();

    //拿到一块block去写，并对拿到的这块block加上写锁
    if(!segment_->AcquireBlockToWrite(msg_size, &wb)){
        AERROR << "acquire block failed.";
        return false;
    }

    ADEBUG << "block index: " << wb.index;
    //拷贝序列化后的数据到wb.buf处
    std::memcpy(wb.buf , ds.data(), msg_size);

    wb.block->set_msg_size(msg_size);


    char* msg_info_addr = reinterpret_cast<char*>(wb.buf) + msg_size;

    //拷贝sender_id
    std::memcpy(msg_info_addr, msg_info.sender_id().data() ,ID_SIZE);
    //拷贝spare_id_
    std::memcpy(msg_info_addr + ID_SIZE , msg_info.spare_id().data() , ID_SIZE);
    //拷贝 seq
    std::memcpy(msg_info_addr + ID_SIZE*2, msg_info.seq_num() , sizeof(uint64_t));


    wb.block->set_msg_info_size(ID_SIZE*2 +sizeof(uint64_t));

    //释放此block的写锁
    segment_->ReleaseWrittenBlock(wb);

    ReadableInfo readable_info(host_id_, wb.index , channel_id_);

    ADEBUG << "Writing sharedmem message: "
         << common::GlobalData::GetChannelById(channel_id_)
         << " to block: " << wb.index;
    //通知接收数据的进程处理数据
    return notifier_->Notify(readable_info);

}



}
}
}


#endif