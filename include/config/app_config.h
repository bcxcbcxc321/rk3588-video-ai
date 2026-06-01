#pragma once

#include <string>

#include "logging/logger.h"

namespace rk_video_ai {

struct AppConfig {
    std::string name = "rk3588_video_ai";
    LogLevel log_level = LogLevel::Info;
    bool enable_console_log = true;
    std::string log_file;
    bool graceful_shutdown = true;
};

class AppConfigLoader {
public:
    static bool loadFromFile(const std::string& path,
                             AppConfig& config,
                             std::string& error_message);

private:
    static bool parseBool(const std::string& text, bool& value);
};

}  // namespace rk_video_ai
