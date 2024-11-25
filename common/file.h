/**
 * @File Name: file.h
 * @brief   操作文件相关的辅助函数
 * @Author : Timer email:330070781@qq.com
 * @Version : 1.0
 * @Creat Date : 2023-12-10
 * 
 */
#ifndef CMW_COMMON_FILE_H_
#define CMW_COMMON_FILE_H_


#include <string>
namespace hnu {
namespace cmw {
namespace common {

/**
 * @brief Get absolute path by concatenating prefix and relative_path.
 * @return The absolute path.
 */
std::string GetAbsolutePath(const std::string &prefix,
                            const std::string &relative_path);

/**
 * @brief Check if the path exists.
 * @param path a file name, such as /a/b/c.txt
 * @return If the path exists.
 */
bool PathExists(const std::string &path);


//给定一个文件路径，获取文件名
std::string GetFileName(const std::string &path,
                        const bool remove_extension = false);
}
}
}


#endif