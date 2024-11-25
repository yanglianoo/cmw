#ifndef CMW_COMMON_CONFPARSE_H_
#define CMW_COMMON_CONFPARSE_H_

#include <string>
#include <cmw/config/cmw_conf.h>

namespace hnu {
namespace cmw {
namespace config {

bool GetCmwConfFromFile(const std::string& file_path, CmwConfig* config);


}
}
}


#endif