#ifndef CMW_CONFIG_CLASSICCONF_H_
#define CMW_CONFIG_CLASSICCONF_H_

#include <string>
#include <vector>

namespace hnu    {
namespace cmw   {
namespace config {

struct ClassicTask {
    std::string name;
    uint32_t prio = 0; // 默认值
    std::string group_name;
};

struct SchedGroup {
    std::string name;
    uint32_t processor_num = 0; // 默认值
    std::string affinity;
    std::string cpuset;
    std::string processor_policy;
    int32_t processor_prio = 0; // 默认值
    std::vector<ClassicTask> tasks;
};

struct ClassicConf {
    std::vector<SchedGroup> groups;
};

}
}
}

#endif