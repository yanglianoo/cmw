#ifndef CMW_CONFIG_ROLEATTRIBUTES_H_
#define CMW_CONFIG_ROLEATTRIBUTES_H_

#include <string>

#include <cmw/config/qos_profile.h>
namespace hnu    {
namespace cmw   {
namespace config {

struct RoleAttributes
{
    std::string host_name;
    std::string host_ip;
    int32_t process_id;

    std::string channel_name;
    uint64_t channel_id;         // hash value of channel_name
    QosProfile qos_profile;
    uint64_t id;

};


}
}
}



#endif