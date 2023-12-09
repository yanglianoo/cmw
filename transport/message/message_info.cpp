#include <cmw/transport/message/message_info.h>

#include <cstring>

namespace hnu    {
namespace cmw   {
namespace transport {

// MessageInfo 二进制的长度 不包含channel_id_
const std::size_t MessageInfo::kSize = 2 * ID_SIZE + sizeof(uint64_t);

MessageInfo::MessageInfo() : sender_id_(false), spare_id_(false) {}

MessageInfo::MessageInfo(const Identity& sender_id, uint64_t seq_num)
    : sender_id_(sender_id), seq_num_(seq_num), spare_id_(false) {}


MessageInfo::MessageInfo(const MessageInfo& another)
    : sender_id_(another.sender_id_),
      channel_id_(another.channel_id_),
      seq_num_(another.seq_num_),
      spare_id_(another.spare_id_) {}


MessageInfo::~MessageInfo() {}    


MessageInfo& MessageInfo::operator=(const MessageInfo& another) {
  if (this != &another) {
    sender_id_ = another.sender_id_;
    channel_id_ = another.channel_id_;
    seq_num_ = another.seq_num_;
    spare_id_ = another.spare_id_;
  }
  return *this;
}

bool MessageInfo::operator==(const MessageInfo& another) const {
  return sender_id_ == another.sender_id_ &&
         channel_id_ == another.channel_id_ && seq_num_ == another.seq_num_ &&
         spare_id_ == another.spare_id_;
}

bool MessageInfo::operator!=(const MessageInfo& another) const {
  return !(*this == another);
}

bool MessageInfo::SerializeTo(std::string* dst) const {
  
  if(dst == nullptr) return false;

  dst->assign(sender_id_.data(), ID_SIZE);
  dst->append(reinterpret_cast<const char*>(&seq_num_), sizeof(seq_num_));
  dst->append(spare_id_.data(), ID_SIZE);

  return true;
}

bool MessageInfo::SerializeTo(char* dst, std::size_t len) const {
  if (dst == nullptr || len < kSize) {
    return false;
  }

  char* ptr = dst;
  std::memcpy(ptr, sender_id_.data(), ID_SIZE);
  ptr += ID_SIZE;
  std::memcpy(ptr, reinterpret_cast<const char*>(&seq_num_), sizeof(seq_num_));
  ptr += sizeof(seq_num_);
  std::memcpy(ptr, spare_id_.data(), ID_SIZE);

  return true;
}

bool MessageInfo::DeserializeFrom(const std::string& src) {
  return DeserializeFrom(src.data(), src.size());
}

bool MessageInfo::DeserializeFrom(const char* src, std::size_t len) {
  if(src == nullptr) return false;
  if (len != kSize) {
    std::cout << "src size mismatch, given[" << len << "] target[" << kSize << "]";
    return false;
  }

  char* ptr = const_cast<char*>(src);
  sender_id_.set_data(ptr);
  ptr += ID_SIZE;
  std::memcpy(reinterpret_cast<char*>(&seq_num_), ptr, sizeof(seq_num_));
  ptr += sizeof(seq_num_);
  spare_id_.set_data(ptr);

  return true;
}

}
}
}



