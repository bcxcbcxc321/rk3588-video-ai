#include "config/app_config.h"

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
}  // namespace

bool AppConfigLoader::loadFromFile(const std::string& path,
                                   AppConfig& config,
                                   std::string& error_message) {
    std::ifstream file(path);
    if (!file.is_open()) {
        error_message = "config file not found or cannot be opened: " + path;
        return false;
    }

    std::unordered_map<std::string, std::string> app_values;
    bool in_app_section = false;
    bool app_section_found = false;
    std::string line;
    int line_number = 0;

    while (std::getline(file, line)) {
        ++line_number;
        const std::string no_comment = stripComment(line);
        const std::string trimmed = trim(no_comment);
        if (trimmed.empty()) {
            continue;
        }

        if (trimmed == "app:") {
            in_app_section = true;
            app_section_found = true;
            continue;
        }

        if (!line.empty() && line[0] != ' ' && line[0] != '\t') {
            in_app_section = false;
        }

        if (!in_app_section) {
            continue;
        }

        const auto colon_pos = trimmed.find(':');
        if (colon_pos == std::string::npos) {
            std::ostringstream oss;
            oss << "invalid app config line " << line_number << ": " << line;
            error_message = oss.str();
            return false;
        }

        const std::string key = trim(trimmed.substr(0, colon_pos));
        const std::string value = unquote(trimmed.substr(colon_pos + 1));
        if (!key.empty()) {
            app_values[key] = value;
        }
    }

    if (!app_section_found) {
        error_message = "missing required section: app";
        return false;
    }

    if (const auto it = app_values.find("name"); it != app_values.end()) {
        config.name = it->second;
    }

    if (config.name.empty()) {
        error_message = "app.name cannot be empty";
        return false;
    }

    if (const auto it = app_values.find("log_level"); it != app_values.end()) {
        LogLevel level;
        if (!parseLogLevel(it->second, level)) {
            error_message = "invalid app.log_level: " + it->second + " (expected debug/info/warn/error)";
            return false;
        }
        config.log_level = level;
    }

    if (const auto it = app_values.find("enable_console_log"); it != app_values.end()) {
        bool enabled = true;
        if (!parseBool(it->second, enabled)) {
            error_message = "invalid app.enable_console_log: " + it->second + " (expected true/false)";
            return false;
        }
        config.enable_console_log = enabled;
    }

    if (const auto it = app_values.find("log_file"); it != app_values.end()) {
        config.log_file = it->second;
    }

    if (const auto it = app_values.find("graceful_shutdown"); it != app_values.end()) {
        bool enabled = true;
        if (!parseBool(it->second, enabled)) {
            error_message = "invalid app.graceful_shutdown: " + it->second + " (expected true/false)";
            return false;
        }
        config.graceful_shutdown = enabled;
    }

    return true;
}

bool AppConfigLoader::parseBool(const std::string& text, bool& value) {
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

}  // namespace rk_video_ai
