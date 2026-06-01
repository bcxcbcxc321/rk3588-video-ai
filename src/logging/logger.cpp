#include "logging/logger.h"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <filesystem>
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

std::string normalize(std::string text) {
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return text;
}

std::string formatLine(LogLevel level,
                       const char* file,
                       int line,
                       const std::string& message) {
    std::ostringstream oss;
    oss << '[' << nowString() << "] "
        << '[' << toString(level) << "] "
        << '[' << baseName(file) << ':' << line << "] "
        << message;
    return oss.str();
}
}  // namespace

Logger& Logger::instance() {
    static Logger logger;
    return logger;
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(g_log_mutex);
    level_ = level;
}

LogLevel Logger::level() const {
    return level_;
}

void Logger::setConsoleEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(g_log_mutex);
    console_enabled_ = enabled;
}

bool Logger::consoleEnabled() const {
    return console_enabled_;
}

bool Logger::setLogFile(const std::string& file_path) {
    std::lock_guard<std::mutex> lock(g_log_mutex);

    try {
        const std::filesystem::path path(file_path);
        if (path.has_parent_path()) {
            std::filesystem::create_directories(path.parent_path());
        }
        file_stream_.open(file_path, std::ios::app);
        return file_stream_.is_open();
    } catch (const std::exception& e) {
        if (console_enabled_) {
            std::cerr << '[' << nowString() << "] [ERROR] [logger.cpp] "
                      << "failed to open log file: " << file_path
                      << ", reason: " << e.what() << std::endl;
        }
        return false;
    }
}

void Logger::closeLogFile() {
    std::lock_guard<std::mutex> lock(g_log_mutex);
    if (file_stream_.is_open()) {
        file_stream_.close();
    }
}

bool Logger::fileEnabled() const {
    return file_stream_.is_open();
}

void Logger::configureFromEnvironment() {
    if (const char* env_level = std::getenv("RK_VIDEO_AI_LOG_LEVEL")) {
        LogLevel parsed_level;
        if (parseLogLevel(env_level, parsed_level)) {
            setLevel(parsed_level);
        }
    }

    if (const char* env_file = std::getenv("RK_VIDEO_AI_LOG_FILE")) {
        if (std::string(env_file).empty()) {
            return;
        }
        setLogFile(env_file);
    }
}

void Logger::log(LogLevel level,
                 const char* file,
                 int line,
                 const std::string& message) {
    std::lock_guard<std::mutex> lock(g_log_mutex);
    if (static_cast<int>(level) < static_cast<int>(level_)) {
        return;
    }

    const std::string line_text = formatLine(level, file, line, message);

    if (console_enabled_) {
        std::ostream& os = (level == LogLevel::Error) ? std::cerr : std::cout;
        os << line_text << std::endl;
    }

    if (file_stream_.is_open()) {
        file_stream_ << line_text << std::endl;
        file_stream_.flush();
    }
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

bool parseLogLevel(const std::string& text, LogLevel& level) {
    const std::string value = normalize(text);
    if (value == "debug") {
        level = LogLevel::Debug;
        return true;
    }
    if (value == "info") {
        level = LogLevel::Info;
        return true;
    }
    if (value == "warn" || value == "warning") {
        level = LogLevel::Warn;
        return true;
    }
    if (value == "error") {
        level = LogLevel::Error;
        return true;
    }
    return false;
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
