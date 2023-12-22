#ifndef CMW_CONFIG_TOPOLOGYCHANGE_H_
#define CMW_CONFIG_TOPOLOGYCHANGE_H_


#include <cstdint>
#include <cmw/config/RoleAttributes.h>
namespace hnu    {
namespace cmw   {
namespace config {

enum ChangeType{
  CHANGE_NODE = 1,
  CHANGE_CHANNEL = 2,
  CHANGE_SERVICE = 3,
  CHANGE_PARTICIPANT = 4,
};

enum OperateType {
  OPT_JOIN = 1,
  OPT_LEAVE = 2,
};

enum RoleType {
  ROLE_NODE = 1,
  ROLE_WRITER = 2,
  ROLE_READER = 3,
  ROLE_SERVER = 4,
  ROLE_CLIENT = 5,
  ROLE_PARTICIPANT = 6,
};

struct ChangeMsg
{
    uint64_t timestamp;
    ChangeType change_type ;
    OperateType operate_type;
    RoleType role_type;
    RoleAttributes role_attr;
};

}
}
}
#endif