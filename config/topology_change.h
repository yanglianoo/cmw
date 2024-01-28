#ifndef CMW_CONFIG_TOPOLOGYCHANGE_H_
#define CMW_CONFIG_TOPOLOGYCHANGE_H_


#include <cmw/serialize/serializable.h>
#include <cmw/serialize/data_stream.h>
#include <cstdint>
#include <cmw/config/RoleAttributes.h>
namespace hnu    {
namespace cmw   {
namespace config {

using namespace serialize;
//改变的类型
enum ChangeType{
  CHANGE_NODE = 1,
  CHANGE_CHANNEL = 2,
  CHANGE_SERVICE = 3,
  CHANGE_PARTICIPANT = 4,
};

//一个角色的动作
enum OperateType {
  OPT_JOIN = 1,
  OPT_LEAVE = 2,
};

//通信平面中角色的类型
enum RoleType {
  ROLE_NODE = 1,        // Node
  ROLE_WRITER = 2,   // Publisher
  ROLE_READER = 3,  // Subscriber
  ROLE_SERVER = 4,      // 
  ROLE_CLIENT = 5,
  ROLE_PARTICIPANT = 6,
};

struct ChangeMsg : public Serializable
{
    uint64_t timestamp;  
    ChangeType change_type ;
    OperateType operate_type;
    RoleType role_type;
    RoleAttributes role_attr;

    SERIALIZE(timestamp,change_type,operate_type,role_type,role_attr)
};

}
}
}
#endif