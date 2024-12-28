#ifndef CMW_CONFIG_COMPONENTCONF_H_
#define CMW_CONFIG_COMPONENTCONF_H_

#include <string>
namespace hnu    {
namespace cmw   {
namespace config {

struct ComponentConfig
{
    std::string name;
    std::string config_file_path;
    std::string flag_file_path;
};

struct TimerComponentConfig {
    std::string name;
    std::string config_file_path;
    std::string flag_file_path;
    uint32_t interval;
};


}
}
}


#endif