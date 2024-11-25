#ifndef CMW_CONFIG_CHOREOGRAPHYCONF_H_
#define CMW_CONFIG_CHOREOGRAPHYCONF_H_

#include <string>
namespace hnu    {
namespace cmw   {
namespace config {

struct ChoreographyTask
{
    std::string name;
    int32_t processor;
    uint32_t prio;
};


struct ChoreographyConf
{
    uint32_t choreography_processor_num;
    std::string choreography_affinity;
    std::string choreography_processor_policy;
    int32_t choreography_processor_prio;
    std::string choreography_cpuset;
    uint32_t pool_processor_num;
    std::string pool_affinity;
    std::string pool_processor_policy;
    int32_t pool_processor_prio;
    std::string pool_cpuset;
    ChoreographyTask tasks;
};


}
}
}

#endif