#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

// 使用 nlohmann::json 简化命名
using json = nlohmann::json;

// 定义结构体
struct InnerThread {
    std::string name;
    std::string cpuset;
    std::string policy;
    uint32_t prio = 0; // 默认值
};

struct ClassicTask {
    std::string name;
    uint32_t prio = 0; // 默认值
    std::string group_name;
};

struct SchedGroup {
    std::string name;
    uint32_t processor_num = 0; // 默认值
    std::string affinity;
    std::string cpuset;
    std::string processor_policy;
    int32_t processor_prio = 0; // 默认值
    std::vector<ClassicTask> tasks;
};

struct ClassicConf {
    std::vector<SchedGroup> groups;
};

struct SchedulerConf {
    std::string policy;
    uint32_t routine_num = 0; // 默认值
    uint32_t default_proc_num = 0; // 默认值
    std::string process_level_cpuset;
    std::vector<InnerThread> threads;
    ClassicConf classic_conf;
};

struct CmwConfig {
    SchedulerConf scheduler_conf;
};

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

// 封装函数：从文件中加载配置并解析到 CmwConfig
bool GetProtoFromFile(const std::string& file_path, CmwConfig* config) {
    if (config == nullptr) {
        std::cerr << "Error: CmwConfig pointer is null." << std::endl;
        return false;
    }

    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Failed to open the JSON file: " << file_path << std::endl;
        return false;
    }

    json j;
    try {
        file >> j; // 读取 JSON 文件
    } catch (const json::exception& e) {
        std::cerr << "Error reading JSON file: " << e.what() << std::endl;
        return false;
    }

    try {
        *config = j.get<CmwConfig>(); // 解析 JSON 到 CmwConfig
    } catch (const json::exception& e) {
        std::cerr << "Error parsing JSON to CmwConfig: " << e.what() << std::endl;
        return false;
    }

    return true; // 成功解析
}

// 示例主函数
int main() {
    CmwConfig config;
    std::string file_path = "cyber.pb.conf";

    if (GetProtoFromFile(file_path, &config)) {
        std::cout << "Scheduler Policy: " << config.scheduler_conf.policy << std::endl;
        std::cout << "routine_num: " << config.scheduler_conf.routine_num << std::endl;
        std::cout << "default_proc_num: " << config.scheduler_conf.default_proc_num << std::endl;
        std::cout << "Process Level Cpuset: " << config.scheduler_conf.process_level_cpuset << std::endl;

        for (const auto& thread : config.scheduler_conf.threads) {
            std::cout << "Thread Name: " << thread.name << ", Cpuset: " << thread.cpuset
                      << ", Policy: " << thread.policy << ", Prio: " << thread.prio << std::endl;
        }

        for (const auto& group : config.scheduler_conf.classic_conf.groups) {
            std::cout << "Group Name: " << group.name << ", Processor Num: " << group.processor_num
                      << ", Affinity: " << group.affinity << ", Cpuset: " << group.cpuset
                      << ", Processor Policy: " << group.processor_policy
                      << ", Processor Prio: " << group.processor_prio << std::endl;

            for (const auto& task : group.tasks) {
                std::cout << "  Task Name: " << task.name << ", Prio: " << task.prio
                          << ", Group Name: " << task.group_name << std::endl;
            }
        }
    } else {
        std::cerr << "Failed to parse the configuration file." << std::endl;
    }

    return 0;
}
