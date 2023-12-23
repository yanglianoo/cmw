#ifndef CMW_CONFIG_QOS_PROFILE_H_
#define CMW_CONFIG_QOS_PROFILE_H_

namespace hnu    {
namespace cmw   {
namespace config {

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


struct QosProfile
{
  QosHistoryPolicy history = HISTORY_KEEP_LAST;
  uint32_t depth = 2; //default = 1 
  uint32_t mps = 3;  // [default = 0];    // messages per second
  QosReliabilityPolicy reliability = RELIABILITY_RELIABLE;
  QosDurabilityPolicy durability = DURABILITY_VOLATILE;

};


}
}
}



#endif
