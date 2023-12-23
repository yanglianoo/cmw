#include <cmw/transport/qos/qos_profile_conf.h>


namespace hnu    {
namespace cmw   {
namespace transport {

QosProfileConf::QosProfileConf() {}

QosProfileConf::~QosProfileConf() {}

QosProfile QosProfileConf::CreateQosProfile(
    const QosHistoryPolicy& history, uint32_t depth, uint32_t mps,
    const QosReliabilityPolicy& reliability,
    const QosDurabilityPolicy& durability) {
  QosProfile qos_profile;
  qos_profile.history = history;
  qos_profile.reliability = reliability;
  qos_profile.durability = durability;
  qos_profile.mps = mps;
  qos_profile.depth = depth;
  return qos_profile;
}

const uint32_t QosProfileConf::QOS_HISTORY_DEPTH_SYSTEM_DEFAULT = 0;
const uint32_t QosProfileConf::QOS_MPS_SYSTEM_DEFAULT = 0;

const QosProfile QosProfileConf::QOS_PROFILE_DEFAULT = CreateQosProfile(
    QosHistoryPolicy::HISTORY_KEEP_LAST, 1, QOS_MPS_SYSTEM_DEFAULT,
    QosReliabilityPolicy::RELIABILITY_RELIABLE,
    QosDurabilityPolicy::DURABILITY_VOLATILE);

const QosProfile QosProfileConf::QOS_PROFILE_SENSOR_DATA = CreateQosProfile(
    QosHistoryPolicy::HISTORY_KEEP_LAST, 5, QOS_MPS_SYSTEM_DEFAULT,
    QosReliabilityPolicy::RELIABILITY_BEST_EFFORT,
    QosDurabilityPolicy::DURABILITY_VOLATILE);

const QosProfile QosProfileConf::QOS_PROFILE_PARAMETERS = CreateQosProfile(
    QosHistoryPolicy::HISTORY_KEEP_LAST, 1000, QOS_MPS_SYSTEM_DEFAULT,
    QosReliabilityPolicy::RELIABILITY_RELIABLE,
    QosDurabilityPolicy::DURABILITY_VOLATILE);

const QosProfile QosProfileConf::QOS_PROFILE_SERVICES_DEFAULT =
    CreateQosProfile(QosHistoryPolicy::HISTORY_KEEP_LAST, 10,
                     QOS_MPS_SYSTEM_DEFAULT,
                     QosReliabilityPolicy::RELIABILITY_RELIABLE,
                     QosDurabilityPolicy::DURABILITY_TRANSIENT_LOCAL);

const QosProfile QosProfileConf::QOS_PROFILE_PARAM_EVENT = CreateQosProfile(
    QosHistoryPolicy::HISTORY_KEEP_LAST, 1000, QOS_MPS_SYSTEM_DEFAULT,
    QosReliabilityPolicy::RELIABILITY_RELIABLE,
    QosDurabilityPolicy::DURABILITY_VOLATILE);

const QosProfile QosProfileConf::QOS_PROFILE_SYSTEM_DEFAULT = CreateQosProfile(
    QosHistoryPolicy::HISTORY_SYSTEM_DEFAULT, QOS_HISTORY_DEPTH_SYSTEM_DEFAULT,
    QOS_MPS_SYSTEM_DEFAULT, QosReliabilityPolicy::RELIABILITY_RELIABLE,
    QosDurabilityPolicy::DURABILITY_TRANSIENT_LOCAL);

const QosProfile QosProfileConf::QOS_PROFILE_TF_STATIC = CreateQosProfile(
    QosHistoryPolicy::HISTORY_KEEP_ALL, 10, QOS_MPS_SYSTEM_DEFAULT,
    QosReliabilityPolicy::RELIABILITY_RELIABLE,
    QosDurabilityPolicy::DURABILITY_TRANSIENT_LOCAL);

const QosProfile QosProfileConf::QOS_PROFILE_TOPO_CHANGE = CreateQosProfile(
    QosHistoryPolicy::HISTORY_KEEP_ALL, 10, QOS_MPS_SYSTEM_DEFAULT,
    QosReliabilityPolicy::RELIABILITY_RELIABLE,
    QosDurabilityPolicy::DURABILITY_TRANSIENT_LOCAL);

    
}
}
}