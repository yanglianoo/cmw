#ifndef CMW_CONFIG_TRANSPORT_CONFIG_H_
#define CMW_CONFIG_TRANSPORT_CONFIG_H_

namespace hnu    {
namespace cmw   {
namespace config {

//传输层使用的通讯方式
enum OptionalMode{
    HYBRID = 0,
    INTRA = 1,
    SHM = 2,
    RTPS = 3,
};


    
}
}
}

#endif