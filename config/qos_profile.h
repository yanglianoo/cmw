#ifndef CMW_CONFIG_QOS_PROFILE_H_
#define CMW_CONFIG_QOS_PROFILE_H_


#include <cmw/serialize/serializable.h>
#include <cmw/serialize/data_stream.h>
namespace hnu    {
namespace cmw   {
namespace config {

using namespace serialize;
enum QosHistoryPolicy {
  HISTORY_SYSTEM_DEFAULT = 0,
  HISTORY_KEEP_LAST = 1,
  HISTORY_KEEP_ALL = 2,
};

enum QosReliabilityPolicy {
  RELIABILITY_SYSTEM_DEFAULT = 0,
  RELIABILITY_RELIABLE = 1,
  RELIABILITY_BEST_EFFORT = 2,
};

enum QosDurabilityPolicy {
  DURABILITY_SYSTEM_DEFAULT = 0,
  DURABILITY_TRANSIENT_LOCAL = 1,
  DURABILITY_VOLATILE = 2,
};

/*Qos配置信息结构体，支持序列化*/
class QosProfile : public Serializable
{
public:
  QosHistoryPolicy history = HISTORY_KEEP_LAST;
  uint32_t depth = 2; //default = 1 
  uint32_t mps = 3;  // [default = 0];    // messages per second
  uint32_t msg_size = 0;
  QosReliabilityPolicy reliability = RELIABILITY_RELIABLE;
  QosDurabilityPolicy durability = DURABILITY_VOLATILE;

  SERIALIZE(history,depth,mps,msg_size,reliability,durability)
};


}
}
}



#endif
