/**
 * @File Name: logstream.h
 * @brief  
 * @Author : Timer email:330070781@qq.com
 * @Version : 1.0
 * @Creat Date : 2024-02-08
 * 
 */
#ifndef CMW_LOG_LOGSTREAM_H_
#define CMW_LOG_LOGSTREAM_H_
#include <sstream>
#include <cmw/log/logger.h>
namespace hnu    {
namespace cmw   {
namespace logger {


class LogStream{
public:
    LogStream(Logger::Level level, const char* file, int line) 
        : m_level(level) , m_file(file) , m_line(line) {}
    ~LogStream(){
        // RALL ,此临时对象析构时将所有的lo
        Logger::Instance()->log(m_level, m_file, m_line, "%s", m_stream.str().c_str());
    }

    template <typename T>
    LogStream& operator<<(const T& msg) {
        m_stream << msg;
        return *this;
    }


    LogStream(const LogStream& other)
    : m_level(other.m_level), m_file(other.m_file), m_line(other.m_line), m_stream(other.m_stream.str()) {
    // 注意：这里我们假设m_file和m_line指向的字符串在LogStream对象生命周期内始终有效
    // 并且拷贝构造函数复制了other的ostringstream的内容到新对象的ostringstream中
    }

private:
    Logger::Level m_level;
    const char* m_file;
    int m_line;
    std::ostringstream m_stream;

};

}
}
}

#endif