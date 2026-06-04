#include "config/stream_config.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <unordered_map>

namespace rk_video_ai {
namespace {

std::string trim(const std::string& text) {
    const auto begin = std::find_if_not(text.begin(), text.end(), [](unsigned char c) {
        return std::isspace(c);
    });
    const auto end = std::find_if_not(text.rbegin(), text.rend(), [](unsigned char c) {
        return std::isspace(c);
    }).base();

    if (begin >= end) {
        return "";
    }
    return std::string(begin, end);
}

std::string stripComment(const std::string& line) {
    bool in_single_quote = false;
    bool in_double_quote = false;

    for (std::size_t i = 0; i < line.size(); ++i) {
        const char c = line[i];
        if (c == '\'' && !in_double_quote) {
            in_single_quote = !in_single_quote;
        } else if (c == '"' && !in_single_quote) {
            in_double_quote = !in_double_quote;
        } else if (c == '#' && !in_single_quote && !in_double_quote) {
            return line.substr(0, i);
        }
    }
    return line;
}

std::string unquote(const std::string& text) {
    std::string value = trim(text);
    if (value.size() >= 2) {
        const char first = value.front();
        const char last = value.back();
        if ((first == '"' && last == '"') || (first == '\'' && last == '\'')) {
            return value.substr(1, value.size() - 2);
        }
    }
    return value;
}

std::string toLower(std::string text) {
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return text;
}

bool isTopLevel(const std::string& line) {
    return !line.empty() && line[0] != ' ' && line[0] != '\t';
}

bool parseKeyValue(const std::string& text,
                   std::string& key,
                   std::string& value,
                   std::string& error_message,
                   int line_number) {
    const auto colon_pos = text.find(':');
    if (colon_pos == std::string::npos) {
        std::ostringstream oss;
        oss << "invalid stream config line " << line_number << ": " << text;
        error_message = oss.str();
        return false;
    }

    key = trim(text.substr(0, colon_pos));
    value = unquote(text.substr(colon_pos + 1));
    return !key.empty();
}

}  // namespace

bool StreamConfigLoader::loadFromFile(const std::string& path,
                                      StreamsConfig& config,
                                      std::string& error_message) {
    std::ifstream file(path);
    if (!file.is_open()) {
        error_message = "streams config file not found or cannot be opened: " + path;
        return false;
    }

    std::vector<std::unordered_map<std::string, std::string>> stream_values;
    std::unordered_map<std::string, std::string> current_stream;
    bool in_streams_section = false;
    bool streams_section_found = false;
    std::string line;
    int line_number = 0;

    auto flushCurrentStream = [&]() {
        if (!current_stream.empty()) {
            stream_values.push_back(current_stream);
            current_stream.clear();
        }
    };

    while (std::getline(file, line)) {
        ++line_number;
        const std::string no_comment = stripComment(line);
        const std::string trimmed = trim(no_comment);
        if (trimmed.empty()) {
            continue;
        }

        if (trimmed == "streams:") {
            in_streams_section = true;
            streams_section_found = true;
            continue;
        }

        if (isTopLevel(line)) {
            in_streams_section = false;
            flushCurrentStream();
        }

        if (!in_streams_section) {
            continue;
        }

        if (trimmed.rfind("- ", 0) == 0) {
            flushCurrentStream();
            const std::string item_text = trim(trimmed.substr(2));
            if (item_text.empty()) {
                continue;
            }

            std::string key;
            std::string value;
            if (!parseKeyValue(item_text, key, value, error_message, line_number)) {
                return false;
            }
            current_stream[key] = value;
            continue;
        }

        std::string key;
        std::string value;
        if (!parseKeyValue(trimmed, key, value, error_message, line_number)) {
            return false;
        }
        current_stream[key] = value;
    }

    flushCurrentStream();

    if (!streams_section_found) {
        error_message = "missing required section: streams";
        return false;
    }

    if (stream_values.empty()) {
        error_message = "streams config must contain at least one stream";
        return false;
    }

    StreamsConfig loaded_config;
    for (std::size_t i = 0; i < stream_values.size(); ++i) {
        const auto& values = stream_values[i];
        StreamConfig stream;

        if (const auto it = values.find("id"); it != values.end()) {
            stream.id = it->second;
        }
        if (const auto it = values.find("name"); it != values.end()) {
            stream.name = it->second;
        }
        if (const auto it = values.find("type"); it != values.end()) {
            stream.type = toLower(it->second);
        }
        if (const auto it = values.find("url"); it != values.end()) {
            stream.url = it->second;
        }
        if (const auto it = values.find("enabled"); it != values.end()) {
            if (!parseBool(it->second, stream.enabled)) {
                error_message = "invalid streams.enabled: " + it->second + " (expected true/false)";
                return false;
            }
        }
        if (const auto it = values.find("loop"); it != values.end()) {
            if (!parseBool(it->second, stream.loop)) {
                error_message = "invalid streams.loop: " + it->second + " (expected true/false)";
                return false;
            }
        }
        if (const auto it = values.find("reconnect"); it != values.end()) {
            if (!parseBool(it->second, stream.reconnect)) {
                error_message = "invalid streams.reconnect: " + it->second + " (expected true/false)";
                return false;
            }
        }
        if (const auto it = values.find("reconnect_interval_ms"); it != values.end()) {
            if (!parseInt(it->second, stream.reconnect_interval_ms)) {
                error_message = "invalid streams.reconnect_interval_ms: " + it->second;
                return false;
            }
        }
        if (const auto it = values.find("read_timeout_ms"); it != values.end()) {
            if (!parseInt(it->second, stream.read_timeout_ms)) {
                error_message = "invalid streams.read_timeout_ms: " + it->second;
                return false;
            }
        }
        if (const auto it = values.find("expected_fps"); it != values.end()) {
            if (!parseDouble(it->second, stream.expected_fps)) {
                error_message = "invalid streams.expected_fps: " + it->second;
                return false;
            }
        }

        if (stream.id.empty()) {
            std::ostringstream oss;
            oss << "streams[" << i << "].id cannot be empty";
            error_message = oss.str();
            return false;
        }
        if (stream.url.empty()) {
            std::ostringstream oss;
            oss << "streams[" << i << "].url cannot be empty";
            error_message = oss.str();
            return false;
        }
        if (stream.type != "file" && stream.type != "rtsp") {
            error_message = "invalid streams.type: " + stream.type + " (expected file/rtsp)";
            return false;
        }

        loaded_config.streams.push_back(stream);
    }

    config = loaded_config;
    return true;
}

bool StreamConfigLoader::parseBool(const std::string& text, bool& value) {
    const std::string normalized = toLower(trim(text));
    if (normalized == "true" || normalized == "yes" || normalized == "1" || normalized == "on") {
        value = true;
        return true;
    }
    if (normalized == "false" || normalized == "no" || normalized == "0" || normalized == "off") {
        value = false;
        return true;
    }
    return false;
}

bool StreamConfigLoader::parseInt(const std::string& text, int& value) {
    try {
        std::size_t parsed = 0;
        const int parsed_value = std::stoi(trim(text), &parsed);
        if (parsed != trim(text).size()) {
            return false;
        }
        value = parsed_value;
        return true;
    } catch (...) {
        return false;
    }
}

bool StreamConfigLoader::parseDouble(const std::string& text, double& value) {
    try {
        std::size_t parsed = 0;
        const std::string normalized = trim(text);
        const double parsed_value = std::stod(normalized, &parsed);
        if (parsed != normalized.size()) {
            return false;
        }
        value = parsed_value;
        return true;
    } catch (...) {
        return false;
    }
}

}  // namespace rk_video_ai
