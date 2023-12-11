#ifndef CMW_EVENT_PERF_EVENT_H_
#define CMW_EVENT_PERF_EVENT_H_

#include <cstdint>
#include <limits>
#include <sstream>
#include <string>


#include <cmw/common/global_data.h>
#include <cmw/common/macros.h>


namespace hnu    {
namespace cmw   {
namespace event {

//定义了三种事件的枚举，包括 调度 通信 
enum class EventType { SCHED_EVENT = 0, TRANS_EVENT = 1, TRY_FETCH_EVENT = 3 };

enum class TransPerf {
  TRANSMIT_BEGIN = 0,
  SERIALIZE = 1,
  SEND = 2,
  MESSAGE_ARRIVE = 3,
  OBTAIN = 4,  // only for shm
  DESERIALIZE = 5,
  DISPATCH = 6,
  NOTIFY = 7,
  FETCH = 8,
  CALLBACK = 9,
  TRANS_END
};

class EventBase {
 public:
  virtual std::string SerializeToString() = 0;

  void set_eid(int eid) { eid_ = eid; }
  void set_etype(int etype) { etype_ = etype; }
  void set_stamp(uint64_t stamp) { stamp_ = stamp; }

  virtual void set_cr_id(uint64_t cr_id) { UNUSED(cr_id); }
  virtual void set_cr_state(int cr_state) { UNUSED(cr_state); }
  virtual void set_proc_id(int proc_id) { UNUSED(proc_id); }
  virtual void set_fetch_res(int fetch_res) { UNUSED(fetch_res); }

  virtual void set_msg_seq(uint64_t msg_seq) { UNUSED(msg_seq); }
  virtual void set_channel_id(uint64_t channel_id) { UNUSED(channel_id); }
  virtual void set_adder(const std::string& adder) { UNUSED(adder); }

 protected:
  int etype_;
  int eid_;
  uint64_t stamp_;
};


class TransportEvent: public EventBase{
    
public:
  TransportEvent()  { etype_ = static_cast<int>(EventType::TRANS_EVENT); }
  std::string SerializeToString() override {
    std::stringstream ss;
    ss << etype_ << "\t";
    ss << eid_ << "\t";
    ss << common::GlobalData::GetChannelById(channel_id_) << "\t";
    ss << msg_seq_ << "\t";
    ss << stamp_ << "\t" ;
    ss << adder_ << "\t";

    return ss.str();
  }
  void set_msg_seq(uint64_t msg_seq) override { msg_seq_ = msg_seq; }
  void set_channel_id(uint64_t channel_id) override {
    channel_id_ = channel_id;
  }
  void set_adder(const std::string& adder) override { adder_ = adder; }


  static std::string ShowTransPerf(TransPerf type) {
    if (type == TransPerf::TRANSMIT_BEGIN) {
      return "TRANSMIT_BEGIN";
    } else if (type == TransPerf::SERIALIZE) {
      return "SERIALIZE";
    } else if (type == TransPerf::SEND) {
      return "SEND";
    } else if (type == TransPerf::MESSAGE_ARRIVE) {
      return "MESSAGE_ARRIVE";
    } else if (type == TransPerf::OBTAIN) {
      return "OBTAIN";
    } else if (type == TransPerf::DESERIALIZE) {
      return "DESERIALIZE";
    } else if (type == TransPerf::DISPATCH) {
      return "DISPATCH";
    } else if (type == TransPerf::NOTIFY) {
      return "NOTIFY";
    } else if (type == TransPerf::FETCH) {
      return "FETCH";
    } else if (type == TransPerf::CALLBACK) {
      return "CALLBACK";
    }
    return "";
  }
private:
  std::string adder_ = "";
  uint64_t msg_seq_ = 0;
  uint64_t channel_id_ = std::numeric_limits<uint64_t>::max();
};

}
}
}
#endif



