#pragma once

#include <fstream>
#include <string>

namespace rk_video_ai {

enum class LogLevel {
    Debug = 0,
    Info,
    Warn,
    Error,
};

class Logger {
public:
    static Logger& instance();

    void setLevel(LogLevel level);
    LogLevel level() const;

    void setConsoleEnabled(bool enabled);
    bool consoleEnabled() const;

    bool setLogFile(const std::string& file_path);
    void closeLogFile();
    bool fileEnabled() const;

    void configureFromEnvironment();

    void log(LogLevel level,
             const char* file,
             int line,
             const std::string& message);

private:
    Logger() = default;

    LogLevel level_ = LogLevel::Info;
    bool console_enabled_ = true;
    std::ofstream file_stream_;
};

const char* toString(LogLevel level);
bool parseLogLevel(const std::string& text, LogLevel& level);
std::string baseName(const char* path);

}  // namespace rk_video_ai

#define LOG_DEBUG(message) ::rk_video_ai::Logger::instance().log(::rk_video_ai::LogLevel::Debug, __FILE__, __LINE__, (message))
#define LOG_INFO(message)  ::rk_video_ai::Logger::instance().log(::rk_video_ai::LogLevel::Info,  __FILE__, __LINE__, (message))
#define LOG_WARN(message)  ::rk_video_ai::Logger::instance().log(::rk_video_ai::LogLevel::Warn,  __FILE__, __LINE__, (message))
#define LOG_ERROR(message) ::rk_video_ai::Logger::instance().log(::rk_video_ai::LogLevel::Error, __FILE__, __LINE__, (message))
