/**
 * @File Name: log.h
 * @brief  
 * @Author : Timer email:330070781@qq.com
 * @Version : 1.0
 * @Creat Date : 2024-02-08
 * 
 */
#ifndef CMW_COMMON_LOG_H_
#define CMW_COMMON_LOG_H_

#include <cmw/log/logger.h>
#include <cmw/log/logstream.h>
using namespace hnu::cmw::logger;


#define ADEBUG Logger::Instance()->logStream(Logger::LOG_DEBUG,__FILE__, __LINE__)
#define AINFO Logger::Instance()->logStream(Logger::LOG_INFO,__FILE__, __LINE__) 
#define AWARN Logger::Instance()->logStream(Logger::LOG_WARN,__FILE__, __LINE__) 
#define AERROR Logger::Instance()->logStream(Logger::LOG_ERROR,__FILE__, __LINE__)
#define AFATAL Logger::Instance()->logStream(Logger::LOG_FATAL,__FILE__, __LINE__)


#define log_debug(format, ...) \
    Logger::Instance()->log(Logger::LOG_DEBUG, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define log_info(format, ...) \
    Logger::Instance()->log(Logger::LOG_INFO, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define log_warn(format, ...) \
    Logger::Instance()->log(Logger::LOG_WARN, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define log_error(format, ...) \
    Logger::Instance()->log(Logger::LOG_ERROR, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define log_fatal(format, ...) \
    Logger::Instance()->log(Logger::LOG_FATAL, __FILE__, __LINE__, format, ##__VA_ARGS__)


#if !defined(RETURN_IF_NULL)
#define RETURN_IF_NULL(ptr)          \
  if (ptr == nullptr) {              \
    AWARN << #ptr << " is nullptr."; \
    return;                          \
  }
#endif


#if !defined(RETURN_VAL_IF_NULL)
#define RETURN_VAL_IF_NULL(ptr, val) \
  if (ptr == nullptr) {              \
    AWARN << #ptr << " is nullptr."; \
    return val;                      \
  }
#endif

#if !defined(RETURN_IF)
#define RETURN_IF(condition)           \
  if (condition) {                     \
    AWARN << #condition << " is met."; \
    return;                            \
  }
#endif

#if !defined(RETURN_VAL_IF)
#define RETURN_VAL_IF(condition, val)  \
  if (condition) {                     \
    AWARN << #condition << " is met."; \
    return val;                        \
  }
#endif





#endif