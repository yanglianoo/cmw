#ifndef CMW_COMMON_ENVIRONMENT_H_
#define CMW_COMMON_ENVIRONMENT_H_

#include <string>
#include <iostream>
#include <cmw/common/log.h>
namespace hnu {
namespace cmw {
namespace common {

/**
 * @brief  获取环境变量
 */
inline std::string GetEnv(const std::string& var_name,
                          const std::string& default_value = "") {
  const char* var = std::getenv(var_name.c_str());
  if (var == nullptr) {
    AWARN << "Environment variable [" << var_name << "] not set, fallback to "
          << default_value;
    return default_value;
  }
  return std::string(var);
}

/**
 * @brief  设置工作空间路径
 */
inline const std::string WorkRoot() {
  std::string work_root = GetEnv("CMW_PATH");
  if (work_root.empty()) {
    work_root = "/cmw";
  }
  return work_root;
}

}
}
}

#endif