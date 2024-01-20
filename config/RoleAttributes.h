#ifndef CMW_CONFIG_ROLEATTRIBUTES_H_
#define CMW_CONFIG_ROLEATTRIBUTES_H_

#include <string>

#include <cmw/config/qos_profile.h>
#include <cmw/serialize/serializable.h>
#include <cmw/serialize/data_stream.h>
namespace hnu    {
namespace cmw   {
namespace config {
using namespace serialize;

/**/
struct RoleAttributes : public Serializable
{
    std::string host_name;       
    std::string host_ip;
    int32_t process_id;

    std::string channel_name;
    uint64_t channel_id;         // hash value of channel_name
    QosProfile qos_profile;
    uint64_t id;

    std::string node_name;
    uint64_t node_id;  // hash value of node_name

    SERIALIZE(host_name,host_ip,process_id,channel_name,qos_profile,id,node_name,node_id)
};


}
}
}



#endif