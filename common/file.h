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






//给定一个文件路径，获取文件名
std::string GetFileName(const std::string &path,
                        const bool remove_extension = false);
}
}
}


#endif