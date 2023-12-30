#ifndef CMW_SERVICE_DISCOVERY_ROLE_ROLE_H_
#define CMW_SERVICE_DISCOVERY_ROLE_ROLE_H_

#include <cmw/config/RoleAttributes.h>
#include <cstdint>
#include <memory>
#include <string>
namespace hnu {
namespace cmw {
namespace discovery{ 

using namespace config;

class RoleBase;
using RolePtr = std::shared_ptr<RoleBase>;
using RoleNode = RoleBase;
using RoleNodePtr = std::shared_ptr<RoleNode>;

class RoleWriter;
using RoleWriterPtr = std::shared_ptr<RoleWriter>;
using RoleReader = RoleWriter;
using RoleReaderPtr = std::shared_ptr<RoleReader>;

class RoleBase {
public:
  RoleBase();
  virtual ~RoleBase() = default;
  explicit RoleBase(const RoleAttributes& attr,
                    uint64_t timestamp_ns = 0);

  virtual bool Match(const RoleAttributes& target_attr) const;    
  bool IsEarlierThan(const RoleBase& other) const;

  const RoleAttributes& attributes() const { return attributes_; }
  void set_attributes(const RoleAttributes& attr) { attributes_ = attr; }

  uint64_t timestamp_ns() const { return timestamp_ns_; }
  void set_timestamp_ns(uint64_t timestamp_ns) { timestamp_ns_ = timestamp_ns; }
protected:
    RoleAttributes attributes_;
    uint64_t timestamp_ns_;
};

class RoleWriter : public RoleBase {
 public:
  RoleWriter() {}
  explicit RoleWriter(const RoleAttributes& attr,
                      uint64_t timestamp_ns = 0);
  virtual ~RoleWriter() = default;

  bool Match(const RoleAttributes& target_attr) const override;
};



}
}
}

#endif