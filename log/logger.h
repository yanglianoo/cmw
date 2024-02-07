
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

        static Logger* instance();
        void open(const string &filename);
        void close();
        void log(Level level, const char* file, int line, const char* format, ...);
        void max(int bytes);
        void level(int level);
        void console(bool console);

    private:
        ~Logger();
        void rotate();

    private:
        string m_filename;
        ofstream m_fout;
        int m_max;
        int m_len;
        int m_level;
        bool m_console;
        static const char* s_level[LOG_COUNT];
        DECLARE_SINGLETON(Logger)
    };


}
}
}

#endif