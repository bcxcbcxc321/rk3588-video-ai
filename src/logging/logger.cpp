#include "logging/logger.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>

namespace rk_video_ai {
namespace {
std::mutex g_log_mutex;

std::string nowString() {
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        now.time_since_epoch())
                        .count() %
                    1000;

    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &time);
#else
    localtime_r(&time, &tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << '.'
        << std::setw(3) << std::setfill('0') << ms;
    return oss.str();
}
}  // namespace

Logger& Logger::instance() {
    static Logger logger;
    return logger;
}

void Logger::setLevel(LogLevel level) {
    level_ = level;
}

LogLevel Logger::level() const {
    return level_;
}

void Logger::log(LogLevel level,
                 const char* file,
                 int line,
                 const std::string& message) {
    if (static_cast<int>(level) < static_cast<int>(level_)) {
        return;
    }

    std::lock_guard<std::mutex> lock(g_log_mutex);
    std::ostream& os = (level == LogLevel::Error) ? std::cerr : std::cout;
    os << '[' << nowString() << "] "
       << '[' << toString(level) << "] "
       << '[' << baseName(file) << ':' << line << "] "
       << message << std::endl;
}

const char* toString(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:
            return "DEBUG";
        case LogLevel::Info:
            return "INFO";
        case LogLevel::Warn:
            return "WARN";
        case LogLevel::Error:
            return "ERROR";
    }
    return "UNKNOWN";
}

std::string baseName(const char* path) {
    if (path == nullptr) {
        return "unknown";
    }

    std::string value(path);
    const auto pos = value.find_last_of("/\\");
    if (pos == std::string::npos) {
        return value;
    }
    return value.substr(pos + 1);
}

}  // namespace rk_video_ai
