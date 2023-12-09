#include <cmw/transport/common/identity.h>

#include <uuid/uuid.h>
#include <cmw/common/util.h>

namespace hnu    {
namespace cmw   {
namespace transport {


/*UUID是一个128位的唯一标识符*/
Identity::Identity(bool need_generate) : hash_value_(0) {
  std::memset(data_, 0, ID_SIZE);
  if (need_generate) {
    uuid_t uuid;
    uuid_generate(uuid);
    std::memcpy(data_, uuid, ID_SIZE);
    Update();
  }
}

/* 拷贝构造函数 */
Identity::Identity(const Identity& rhs) {
  std::memcpy(data_, rhs.data_, ID_SIZE);
  hash_value_ = rhs.hash_value_;
}


Identity::~Identity() {}

/*重载 = */
Identity& Identity::operator=(const Identity& rhs) {
  if (this != &rhs) {
    std::memcpy(data_, rhs.data_, ID_SIZE);
    hash_value_ = rhs.hash_value_;
  }
  return *this;
}


bool Identity::operator==(const Identity& rhs) const {
  return std::memcmp(data_, rhs.data_, ID_SIZE) == 0;
}


std::string Identity::ToString() const { return std::to_string(hash_value_); }

size_t Identity::Length() const { return ID_SIZE; }

uint64_t Identity::HashValue() const { return hash_value_; }

void Identity::Update() {
  hash_value_ = common::Hash(std::string(data_, ID_SIZE));
}


}
}
}

