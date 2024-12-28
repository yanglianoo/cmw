#ifndef CMW_COMPONENT_COMPONENT_BASE_H_
#define CMW_COMPONENT_COMPONENT_BASE_H_


#include <atomic>
#include <memory>
#include <string>
#include <vector>

#include <cmw/config/component_conf.h>

namespace hnu {
namespace cmw {

using hnu::cmw::config::ComponentConfig;
using hnu::cmw::config::TimerComponentConfig;

class ComponentBase : public std::enable_shared_from_this<ComponentBase>{
public:

protected:
    std::atomic<bool> is_shutdown_ = {false};
    std::string config_file_path_ = "";

};

}
}


#endif