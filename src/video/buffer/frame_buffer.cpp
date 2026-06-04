#include "video/buffer/frame_buffer.h"

namespace rk_video_ai {

bool FrameBuffer::write(const Frame& frame) {
    if (frame.image.empty()) {
        return false;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    latest_frame_ = frame;
    latest_frame_.image = frame.image.clone();
    has_frame_ = true;
    return true;
}

bool FrameBuffer::read(Frame& frame) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!has_frame_) {
        return false;
    }

    frame = latest_frame_;
    frame.image = latest_frame_.image.clone();
    return true;
}

bool FrameBuffer::empty() {
    std::lock_guard<std::mutex> lock(mutex_);
    return !has_frame_;
}

void FrameBuffer::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    latest_frame_ = Frame{};
    has_frame_ = false;
}

}  // namespace rk_video_ai
