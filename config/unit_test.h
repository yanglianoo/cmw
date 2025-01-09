#ifndef CMW_CONFIG_UNIT_TEST_H_
#define CMW_CONFIG_UNIT_TEST_H_

#include <string>
#include <cmw/serialize/serializable.h>
#include <cmw/serialize/data_stream.h>

namespace hnu    {
namespace cmw   {
namespace config {

using namespace serialize;

struct UnitTest : public Serializable
{
    std::string class_name;
    std::string case_name;
    SERIALIZE(class_name,case_name)
};

struct Chatter : public Serializable{
    uint64_t timestamp;
    uint64_t lidar_timestamp;
    uint64_t seq;
    std::string content;
    SERIALIZE(timestamp,lidar_timestamp,seq,content)
};


}
}
}
#endif