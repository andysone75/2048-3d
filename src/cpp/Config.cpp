#include "Config.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include "Utils.h"

#define CONFIG_PATH "config/main.conf"

std::string trim_utf8(const std::string& utf8_str) {
    std::u32string utf32 = Utils::utf8_to_utf32(utf8_str);

    auto is_unicode_space = [](char32_t c) {
        return c == U' ' || c == U'\t' || c == U'\n' ||
            c == U'\r' || c == U'\f' || c == U'\v';
        };

    utf32.erase(
        utf32.begin(),
        std::find_if(utf32.begin(), utf32.end(), [&](char32_t c) {
            return !is_unicode_space(c);
            })
    );

    utf32.erase(
        std::find_if(utf32.rbegin(), utf32.rend(), [&](char32_t c) {
            return !is_unicode_space(c);
            }).base(),
                utf32.end()
                );

    std::string result;
    for (char32_t c : utf32) {
        if (c <= 0x7F) {  // ASCII
            result.push_back(static_cast<char>(c));
        }
        else if (c <= 0x7FF) {  // 2-byte UTF-8
            result.push_back(static_cast<char>(0xC0 | ((c >> 6) & 0x1F)));
            result.push_back(static_cast<char>(0x80 | (c & 0x3F)));
        }
        else if (c <= 0xFFFF) {  // 3-byte UTF-8
            result.push_back(static_cast<char>(0xE0 | ((c >> 12) & 0x0F)));
            result.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (c & 0x3F)));
        }
        else if (c <= 0x10FFFF) {  // 4-byte UTF-8
            result.push_back(static_cast<char>(0xF0 | ((c >> 18) & 0x07)));
            result.push_back(static_cast<char>(0x80 | ((c >> 12) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (c & 0x3F)));
        }
    }
    return result;
}

void Config::initialize() {
    std::ifstream file(CONFIG_PATH, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << CONFIG_PATH << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#')
            continue;

        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            key = trim_utf8(key);

            std::string value = line.substr(colonPos + 1);
            value = trim_utf8(value);

            if (!key.empty() && !value.empty()) {
                options[key] = value;
            }
        }
    }

    file.close();
}