#include <cmw/log/logger.h>

#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <stdexcept>

namespace hnu    {
namespace cmw   {
namespace logger {


const char* Logger::s_level[LOG_COUNT] =
{
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL"
};


Logger::Logger() : m_max(0), m_len(0), m_level(LOG_DEBUG), m_console(true)
{
}

Logger::~Logger()
{
    close();
}



void Logger::open(const string &filename)
{
    m_filename = filename;
    m_fout.open(filename ,std::ios::app); //以追加模式打开一个文件,open函数不会创建文件夹
    if (m_fout.fail())
    {
        throw std::logic_error("open log file failed: " + filename);
    }
    m_fout.seekp(0, std::ios::end); //移动文件写指针到文件的末尾
    m_len = m_fout.tellp();     //返回当前写指针在文件中的位置，即从文件开始到写指针当前位置的字节数
}

void Logger::close()
{
    m_fout.close();
}

void Logger::log(Level level, const char* file, int line, const char* format, ...)
{
    if (m_level > level)
    {
        return;
    }

    if (m_fout.fail())
    {
        return;
    }

    ostringstream oss;
    time_t ticks = time(NULL);
    struct tm* ptm = localtime(&ticks);
    char timestamp[32];
    memset(timestamp, 0, sizeof(timestamp));
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", ptm);

    int len = 0;
    //计算格式化所需缓冲区大小
    const char * fmt = "%s %s %s:%d ";
    len = snprintf(NULL, 0, fmt, timestamp, s_level[level], file, line);
    if (len > 0)
    {
        char * buffer = new char[len + 1];
        //格式化字符串
        snprintf(buffer, len + 1, fmt, timestamp, s_level[level], file, line);
        buffer[len] = 0;
        oss << buffer;
        delete [] buffer;
        m_len += len;
    }


    va_list arg_ptr;
    va_start(arg_ptr, format);
    //计算可变参数的长度
    len = vsnprintf(NULL, 0, format, arg_ptr);
    va_end(arg_ptr);
    if (len > 0)
    {
        char * content = new char[len + 1];
        va_start(arg_ptr, format);
        //格式化可变参数
        vsnprintf(content, len + 1, format, arg_ptr);
        va_end(arg_ptr);
        content[len] = 0;
        oss << content;
        delete [] content;
        m_len += len;
    }

    oss << "\n";
    const string & str = oss.str();
    if (m_console)
    {
        std::cout << str << std::endl;
    }
    m_fout << str;
    //确保数据立即写入到文件中，而不是保存在缓存中
    m_fout.flush();

    if (m_max > 0 && m_len >= m_max)
    {
        rotate();
    }
}

//限制文件大小
void Logger::max(int bytes)
{
    m_max = bytes;
}

void Logger::level(int level)
{
    m_level = level;
}

void Logger::console(bool console)
{
    m_console = console;
}

//如果超出文件大小则新创建一个文件
void Logger::rotate()
{
    close();
    time_t ticks = time(NULL);
    struct tm* ptm = localtime(&ticks);
    char timestamp[32];
    memset(timestamp, 0, sizeof(timestamp));
    strftime(timestamp, sizeof(timestamp), ".%Y-%m-%d_%H-%M-%S", ptm);
    string filename = m_filename + timestamp;
    if (rename(m_filename.c_str(), filename.c_str()) != 0)
    {
        throw std::logic_error("rename log file failed: " + string(strerror(errno)));
    }
    open(m_filename);
}


}
}
}