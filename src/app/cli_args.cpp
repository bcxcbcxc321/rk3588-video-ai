#include "app/cli_args.h"

#include <sstream>
#include <string>

namespace rk_video_ai {

bool parseCliArgs(int argc, char** argv, CliArgs& args, std::string& error_message) {
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            args.show_help = true;
            return true;
        }

        if (arg == "--config") {
            if (i + 1 >= argc) {
                error_message = "missing value after --config";
                return false;
            }
            args.config_path = argv[++i];
            continue;
        }

        constexpr const char* prefix = "--config=";
        if (arg.rfind(prefix, 0) == 0) {
            args.config_path = arg.substr(std::string(prefix).size());
            if (args.config_path.empty()) {
                error_message = "--config value cannot be empty";
                return false;
            }
            continue;
        }

        error_message = "unknown argument: " + arg;
        return false;
    }

    return true;
}

std::string buildHelpText(const char* program_name) {
    std::ostringstream oss;
    oss << "Usage: " << (program_name ? program_name : "rk_video_ai") << " [options]\n\n"
        << "Options:\n"
        << "  --config <path>   Path to app yaml config. Default: configs/app.yaml\n"
        << "  -h, --help        Show this help message\n\n"
        << "Environment:\n"
        << "  RK_VIDEO_AI_LOG_LEVEL=debug|info|warn|error\n"
        << "  RK_VIDEO_AI_LOG_FILE=output/logs/app.log\n";
    return oss.str();
}

}  // namespace rk_video_ai
