#pragma once

#include <string>
#include <vector>

namespace rk_video_ai {

struct StreamConfig {
    std::string id;
    std::string name;
    std::string type = "file";
    std::string url;
    bool enabled = true;
    bool loop = false;
    bool reconnect = false;
    int reconnect_interval_ms = 3000;
    int read_timeout_ms = 5000;
    double expected_fps = 0.0;
};

struct StreamsConfig {
    std::vector<StreamConfig> streams;
};

class StreamConfigLoader {
public:
    static bool loadFromFile(const std::string& path,
                             StreamsConfig& config,
                             std::string& error_message);

private:
    static bool parseBool(const std::string& text, bool& value);
    static bool parseInt(const std::string& text, int& value);
    static bool parseDouble(const std::string& text, double& value);
};

}  // namespace rk_video_ai
