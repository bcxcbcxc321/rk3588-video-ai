#include "app/cli_args.h"
#include "config/app_config.h"
#include "logging/logger.h"

#include <iostream>

int main(int argc, char** argv) {
    rk_video_ai::CliArgs cli_args;
    std::string cli_error;
    if (!rk_video_ai::parseCliArgs(argc, argv, cli_args, cli_error)) {
        std::cerr << "Argument error: " << cli_error << "\n\n"
                  << rk_video_ai::buildHelpText(argv[0]);
        return 2;
    }

    if (cli_args.show_help) {
        std::cout << rk_video_ai::buildHelpText(argv[0]);
        return 0;
    }

    rk_video_ai::AppConfig app_config;
    std::string config_error;
    if (!rk_video_ai::AppConfigLoader::loadFromFile(cli_args.config_path, app_config, config_error)) {
        LOG_ERROR(config_error);
        return 1;
    }

    auto& logger = rk_video_ai::Logger::instance();
    logger.setLevel(app_config.log_level);
    logger.setConsoleEnabled(app_config.enable_console_log);
    if (!app_config.log_file.empty() && !logger.setLogFile(app_config.log_file)) {
        LOG_ERROR("failed to initialize log file: " + app_config.log_file);
        return 1;
    }
    logger.configureFromEnvironment();

    LOG_INFO("loaded config: " + cli_args.config_path);
    LOG_INFO("app.name = " + app_config.name);
    LOG_INFO(std::string("app.log_level = ") + rk_video_ai::toString(app_config.log_level));
    LOG_INFO(std::string("app.enable_console_log = ") + (app_config.enable_console_log ? "true" : "false"));
    LOG_INFO("app.log_file = " + (app_config.log_file.empty() ? std::string("<disabled>") : app_config.log_file));
    LOG_INFO(std::string("app.graceful_shutdown = ") + (app_config.graceful_shutdown ? "true" : "false"));
    LOG_INFO("RK3588 Video AI project skeleton exited normally");
    return 0;
}
