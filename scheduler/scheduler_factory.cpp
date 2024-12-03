#include <cmw/scheduler/scheduler_factory.h>
#include <atomic>
#include <string>
#include <unordered_map>
#include <cmw/config/conf_parse.h>

namespace hnu    {
namespace cmw   {
namespace scheduler {

using hnu::cmw::common::GetAbsolutePath;
using hnu::cmw::config::GetCmwConfFromFile;
using hnu::cmw::common::GlobalData;
using hnu::cmw::common::PathExists;
using hnu::cmw::common::WorkRoot;

namespace {
    std::atomic<Scheduler*> instance = {nullptr};
    std::mutex mutex;
}

Scheduler* Instance() {
    Scheduler* obj = instance.load(std::memory_order_acquire);
    if(obj == nullptr){
        std::lock_guard<std::mutex> lock(mutex);

        obj = instance.load(std::memory_order_relaxed);
        if(obj == nullptr){

            //读取配置文件
            std::string policy;
            auto config_file = std::string("conf/") + 
                                GlobalData::Instance()->ProcessGroup() + ".conf";
            hnu::cmw::config::CmwConfig config;
            if(PathExists(config_file) && GetCmwConfFromFile(config_file, &config)){
                AINFO << "Scheduler conf " << config_file << " found and used.";
                policy = config.scheduler_conf.policy;
            } else {
                auto config_path = GetAbsolutePath(WorkRoot(), config_file);
                if(PathExists(config_path) && GetCmwConfFromFile(config_path, &config)){
                    AINFO << "Scheduler conf " << config_path << " found and used.";
                    policy = config.scheduler_conf.policy;
                } else {
                    policy = "classic";
                    AWARN << "No scheduler conf " << config_file
                    << " found, use default.";
                }
            }
            //new一个 Scheduler
            if(!policy.compare("classic")){
                obj = new SchedulerClassic();
                AINFO << "new SchedulerClassic() ";
            } else if (!policy.compare("choreography")){

            } else {
                AWARN << "Invalid scheduler policy: " << policy;
                obj = new SchedulerClassic();
            }
            //将新建的 Scheduler 存到 instance 指针中
            instance.store(obj, std::memory_order_release);
        }
    }
    return obj;
}

void CleanUp(){
    Scheduler* obj = instance.load(std::memory_order_acquire);
    if (obj != nullptr) {
       // obj->Shutdown();
    }
}


}
}
}