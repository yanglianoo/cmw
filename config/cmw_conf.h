#ifndef CMW_CONFIG_CMWCONF_H_
#define CMW_CONFIG_CMWCONF_H_

#include <cmw/config/scheduler_conf.h>
#include <cmw/config/transport_config.h>
#include <nlohmann/json.hpp>

namespace hnu    {
namespace cmw   {
namespace config {

struct CmwConfig
{
    SchedulerConf scheduler_conf;
    TransportConf transport_conf;
};


// nlohmann::json 序列化支持
void from_json(const nlohmann::json& j, InnerThread& thread);
void from_json(const nlohmann::json& j, ClassicTask& task);
void from_json(const nlohmann::json& j, SchedGroup& group);
void from_json(const nlohmann::json& j, ClassicConf& classic_conf);
void from_json(const nlohmann::json& j, SchedulerConf& scheduler_conf);
void from_json(const nlohmann::json& j, CmwConfig& cmw_config);

}
}
}

#endif