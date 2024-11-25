#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include <cmw/config/conf_parse.h>
#include <cmw/common/log.h>



namespace hnu {
namespace cmw {
namespace config {

using json = nlohmann::json;
// from_json 函数定义
void from_json(const json& j, InnerThread& thread) {
    if (j.contains("name")) j.at("name").get_to(thread.name);
    if (j.contains("cpuset")) j.at("cpuset").get_to(thread.cpuset);
    if (j.contains("policy")) j.at("policy").get_to(thread.policy);
    if (j.contains("prio")) j.at("prio").get_to(thread.prio);
}

void from_json(const json& j, ClassicTask& task) {
    if (j.contains("name")) j.at("name").get_to(task.name);
    if (j.contains("prio")) j.at("prio").get_to(task.prio);
    if (j.contains("group_name")) j.at("group_name").get_to(task.group_name);
}

void from_json(const json& j, SchedGroup& group) {
    if (j.contains("name")) j.at("name").get_to(group.name);
    if (j.contains("processor_num")) j.at("processor_num").get_to(group.processor_num);
    if (j.contains("affinity")) j.at("affinity").get_to(group.affinity);
    if (j.contains("cpuset")) j.at("cpuset").get_to(group.cpuset);
    if (j.contains("processor_policy")) j.at("processor_policy").get_to(group.processor_policy);
    if (j.contains("processor_prio")) j.at("processor_prio").get_to(group.processor_prio);
    if (j.contains("tasks")) group.tasks = j.at("tasks").get<std::vector<ClassicTask>>();
}

void from_json(const json& j, ClassicConf& classic_conf) {
    if (j.contains("groups")) classic_conf.groups = j.at("groups").get<std::vector<SchedGroup>>();
}

void from_json(const json& j, SchedulerConf& scheduler_conf) {
    if (j.contains("policy")) j.at("policy").get_to(scheduler_conf.policy);
    if (j.contains("routine_num")) j.at("routine_num").get_to(scheduler_conf.routine_num);
    if (j.contains("default_proc_num")) j.at("default_proc_num").get_to(scheduler_conf.default_proc_num);
    if (j.contains("process_level_cpuset")) j.at("process_level_cpuset").get_to(scheduler_conf.process_level_cpuset);
    if (j.contains("threads")) scheduler_conf.threads = j.at("threads").get<std::vector<InnerThread>>();
    if (j.contains("classic_conf")) scheduler_conf.classic_conf = j.at("classic_conf").get<ClassicConf>();
}

void from_json(const json& j, CmwConfig& cmw_config) {
    if (j.contains("scheduler_conf")) j.at("scheduler_conf").get_to(cmw_config.scheduler_conf);
}

bool GetCmwConfFromFile(const std::string& file_path, CmwConfig* config){
    if (config == nullptr) {
        AERROR << "Error: CmwConfig pointer is null.";
        return false;
    }
    std::ifstream file(file_path);
    if (!file.is_open()) {
        AERROR << "Failed to open the JSON file: " << file_path;
        return false;
    }

    json j;
    file >> j;
    *config = j.get<CmwConfig>();

    return true;
}


}
}
}
