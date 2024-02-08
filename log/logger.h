
#ifndef CMW_LOG_LOGGER_H_
#define CMW_LOG_LOGGER_H_
#include <iostream>
#include <string>
#include <cmw/common/macros.h>

using std::string;

#include <sstream>
using std::ostringstream;

#include <fstream>
using std::ofstream;



namespace hnu    {
namespace cmw   {
namespace logger {

class LogStream;
    class Logger
    {
    public:
        enum Level
        {
            LOG_DEBUG = 0,
            LOG_INFO,
            LOG_WARN,
            LOG_ERROR,
            LOG_FATAL,
            LOG_COUNT
        };

        void open(const string &filename);
        void close();
        void log(Level level, const char* file, int line, const char* format, ...);
        void max(int bytes);
        void level(int level);
        void console(bool console);

        template <typename T>
        Logger& operator<<(const T& msg) {
            m_stream << msg;
            return *this;
        }
        //以流式写入
        LogStream logStream(Level level,const char* file, int line);
    private:
        ~Logger();
        void rotate();

    private:
        string m_filename;
        ofstream m_fout;
        std::ostringstream m_stream;
        int m_max;
        int m_len;
        int m_level;
        bool m_console;
        static const char* s_level[LOG_COUNT];
        DECLARE_SINGLETON(Logger);
    };


}
}
}

#endif