#include <cmw/init.h>
#include <cmw/common/log.h>
#include <string>
namespace hnu {
namespace cmw {


bool Init(const char* binary_name){
    //初始化日志
    std::string logfile_name = (std::string)binary_name + ".log";

    Logger_Init(logfile_name);
}

}
}