#include "app/cli_args.h"
#include "config/app_config.h"
#include "config/stream_config.h"
#include "logging/logger.h"
#include "video/buffer/frame_buffer.h"

#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>

namespace {

std::string formatDouble(double value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << value;
    return oss.str();
}

const rk_video_ai::StreamConfig* findFirstLocalVideo(const rk_video_ai::StreamsConfig& config) {
    for (const auto& stream : config.streams) {
        if (stream.enabled && stream.type == "file") {
            return &stream;
        }
    }
    return nullptr;
}

bool readLocalVideoDemo(const rk_video_ai::StreamConfig& stream) {
    LOG_INFO("local mp4 path = " + stream.url);

    if (!std::filesystem::exists(stream.url)) {
        LOG_ERROR("video file does not exist: " + stream.url);
        return false;
    }

    cv::VideoCapture cap(stream.url);
    if (!cap.isOpened()) {
        LOG_ERROR("failed to open video: " + stream.url);
        return false;
    }

    const int width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    const int height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    const double fps = cap.get(cv::CAP_PROP_FPS);
    const int total_frames = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));

    LOG_INFO("video.width = " + std::to_string(width));
    LOG_INFO("video.height = " + std::to_string(height));
    LOG_INFO("video.fps = " + formatDouble(fps));
    LOG_INFO("video.total_frames = " + std::to_string(total_frames));

    std::filesystem::create_directories("output/frames");

    cv::Mat frame;
    rk_video_ai::FrameBuffer frame_buffer;
    int frame_index = 0;
    bool saved_first_frame = false;

    while (cap.read(frame)) {
        ++frame_index;

        rk_video_ai::Frame current_frame;
        current_frame.image = frame;
        current_frame.timestamp = std::chrono::steady_clock::now();
        current_frame.channel_id = 0;
        current_frame.frame_index = frame_index;

        if (!frame_buffer.write(current_frame)) {
            LOG_ERROR("failed to write frame to buffer");
            return false;
        }

        rk_video_ai::Frame latest_frame;
        if (!frame_buffer.read(latest_frame)) {
            LOG_ERROR("failed to read latest frame from buffer");
            return false;
        }

        if (!saved_first_frame) {
            const std::string frame_path = "output/frames/stage_2_1_first_frame.jpg";
            cv::imwrite(frame_path, latest_frame.image);
            LOG_INFO("saved first frame: " + frame_path);
            saved_first_frame = true;
        }

        if (frame_index == 1 || frame_index % 30 == 0) {
            LOG_INFO("read frame index = " + std::to_string(latest_frame.frame_index));
        }
    }

    if (frame_index == 0) {
        LOG_ERROR("video opened, but no frame was read");
        return false;
    }

    LOG_INFO("read video finished, frames = " + std::to_string(frame_index));
    return true;
}

}  // namespace

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

    rk_video_ai::StreamsConfig streams_config;
    std::string streams_error;
    if (!rk_video_ai::StreamConfigLoader::loadFromFile(cli_args.streams_config_path,
                                                       streams_config,
                                                       streams_error)) {
        LOG_ERROR(streams_error);
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
    LOG_INFO("loaded streams config: " + cli_args.streams_config_path);
    LOG_INFO("app.name = " + app_config.name);
    LOG_INFO(std::string("app.log_level = ") + rk_video_ai::toString(app_config.log_level));
    LOG_INFO(std::string("app.enable_console_log = ") + (app_config.enable_console_log ? "true" : "false"));
    LOG_INFO("app.log_file = " + (app_config.log_file.empty() ? std::string("<disabled>") : app_config.log_file));
    LOG_INFO(std::string("app.graceful_shutdown = ") + (app_config.graceful_shutdown ? "true" : "false"));

    for (const auto& stream : streams_config.streams) {
        LOG_INFO("stream." + stream.id + ".type = " + stream.type);
        LOG_INFO("stream." + stream.id + ".enabled = " + (stream.enabled ? std::string("true") : "false"));
        LOG_INFO("stream." + stream.id + ".url = " + stream.url);
    }

    const rk_video_ai::StreamConfig* local_video = findFirstLocalVideo(streams_config);
    if (local_video == nullptr) {
        LOG_ERROR("no enabled local file stream found in streams config");
        return 1;
    }

    if (!readLocalVideoDemo(*local_video)) {
        return 1;
    }

    LOG_INFO("RK3588 Video AI project skeleton exited normally");
    return 0;
}
