#include "logging/logger.h"

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    auto& logger = rk_video_ai::Logger::instance();
    logger.configureFromEnvironment();

    LOG_INFO("RK3588 Video AI project skeleton started");
    LOG_DEBUG("debug log is enabled");
    LOG_WARN("configuration module is not implemented yet; using skeleton defaults");
    LOG_ERROR("sample error log for Stage 1.2 validation; no real failure occurred");
    LOG_INFO("RK3588 Video AI project skeleton exited normally");
    return 0;
}
