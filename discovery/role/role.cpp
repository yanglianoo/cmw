#include <cmw/discovery/role/role.h>


namespace hnu {
namespace cmw {
namespace discovery{ 

RoleBase::RoleBase() : timestamp_ns_(0) {}

RoleBase::RoleBase(const RoleAttributes& attr, uint64_t timestamp_ns)
    : attributes_(attr), timestamp_ns_(timestamp_ns) {}

bool RoleBase::Match(const RoleAttributes& target_attr) const {
  if (target_attr.node_id != attributes_.node_id) {
    return false;
  }

  if (target_attr.process_id != attributes_.process_id) {
    return false;
  }

  if (target_attr.host_name != attributes_.host_name) {
    return false;
  }

  return true;
}

bool RoleBase::IsEarlierThan(const RoleBase& other) const {
  return timestamp_ns_ < other.timestamp_ns();
}

RoleWriter::RoleWriter(const RoleAttributes& attr, uint64_t timestamp_ns)
    : RoleBase(attr, timestamp_ns) {}

bool RoleWriter::Match(const RoleAttributes& target_attr) const {
  if (target_attr.channel_id != attributes_.channel_id) {
    return false;
  }

  if (target_attr.id!= attributes_.id) {
    return false;
  }

  return RoleBase::Match(target_attr);
}

}
}
}