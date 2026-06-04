#pragma once

#include <chrono>
#include <cstdint>
#include <mutex>

#include <opencv2/core.hpp>

namespace rk_video_ai {

struct Frame {
    cv::Mat image;
    std::chrono::steady_clock::time_point timestamp;
    int channel_id = 0;
    int64_t frame_index = 0;
};

class FrameBuffer {
public:
    bool write(const Frame& frame);
    bool read(Frame& frame);
    bool empty();
    void clear();

private:
    std::mutex mutex_;
    Frame latest_frame_;
    bool has_frame_ = false;
};

}  // namespace rk_video_ai
