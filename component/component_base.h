#ifndef CMW_COMPONENT_COMPONENT_BASE_H_
#define CMW_COMPONENT_COMPONENT_BASE_H_


#include <atomic>
#include <memory>
#include <string>
#include <vector>

#include <cmw/config/component_conf.h>
#include <cmw/node/subscriber.h>

namespace hnu {
namespace cmw {

using hnu::cmw::config::ComponentConfig;
using hnu::cmw::config::TimerComponentConfig;

class ComponentBase : public std::enable_shared_from_this<ComponentBase>{
public:
    template <typename M>
    using Subscriber = cmw::Subscriber<M>;

    virtual ~ComponentBase() {}

    virtual bool Initialize(const ComponentConfig& config) { return false; }
    virtual bool Initialize(const TimerComponentConfig& config) { return false; }

    virtual void Shutdown() {
        if(is_shutdown_.exchange(true)){
            return;
        }

        Clear();

        for(auto& subscriber : subscribers_){
            subscriber->Shutdown();
        }

    }

    template <typename T>
    bool GetProtoConfig(T* config) const {
        
    }

protected:
    virtual bool Init() = 0;
    virtual void Clear() { return; }
    const std::string& ConfigFilePath() const { return config_file_path_; }

    void LoadConfigFiles(const ComponentConfig& config) {

    }

    void LoadConfigFiles(const TimerComponentConfig& config){

    }

protected:
    std::atomic<bool> is_shutdown_ = {false};
    std::string config_file_path_ = "";
    std::vector<std::shared_ptr<SubscriberBase>> subscribers_;
};

}
}


#endif