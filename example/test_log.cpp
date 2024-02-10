#include <cmw/common/log.h>
using namespace hnu::cmw::logger;


int main()
{
    Logger_Init("test.log");
    log_info("c info");
    log_warn("c warn");
    //流式log
    AINFO << "hello test " << 12 ; 
    ADEBUG << " test debug";
    AWARN << "test warn";
    return 0;
}


