#ifndef CMW_CONFIG_SCHEDULERCONF_H_
#define CMW_CONFIG_SCHEDULERCONF_H_

#include <cmw/config/classic_conf.h>
#include <string>
#include <vector>

namespace hnu    {
namespace cmw   {
namespace config {

struct InnerThread {
    std::string name;
    std::string cpuset;
    std::string policy;
    uint32_t prio = 0; // 默认值
};

struct SchedulerConf {
    std::string policy;
    uint32_t routine_num = 0; // 默认值
    uint32_t default_proc_num = 0; // 默认值
    std::string process_level_cpuset;
    std::vector<InnerThread> threads;
    ClassicConf classic_conf;
};

}
}
}
#endif