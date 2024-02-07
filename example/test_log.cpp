#include <cmw/log/logger.h>

using namespace hnu::cmw::logger;


int main()
{
    Logger::Instance()->open("test.log");
    log_debug("test_debug");
    log_info("test_info");
    return 0;
}


