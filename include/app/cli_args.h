#pragma once

#include <string>

namespace rk_video_ai {

struct CliArgs {
    std::string config_path = "configs/app.yaml";
    std::string streams_config_path = "configs/streams.yaml";
    bool show_help = false;
};

bool parseCliArgs(int argc, char** argv, CliArgs& args, std::string& error_message);
std::string buildHelpText(const char* program_name);

}  // namespace rk_video_ai
