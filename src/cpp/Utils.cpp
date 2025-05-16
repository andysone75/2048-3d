#include "Utils.h"
#include <stdexcept>

std::u32string Utils::utf8_to_utf32(const std::string& utf8) {
    std::u32string utf32;
    size_t i = 0;
    while (i < utf8.size()) {
        uint8_t c = utf8[i++];
        char32_t codepoint = 0;

        if ((c & 0x80) == 0x00) {  // 1-byte character (ASCII)
            codepoint = c;
        }
        else if ((c & 0xE0) == 0xC0) {  // 2-byte character
            codepoint = (c & 0x1F) << 6;
            codepoint |= (utf8[i++] & 0x3F);
        }
        else if ((c & 0xF0) == 0xE0) {  // 3-byte character
            codepoint = (c & 0x0F) << 12;
            codepoint |= (utf8[i++] & 0x3F) << 6;
            codepoint |= (utf8[i++] & 0x3F);
        }
        else if ((c & 0xF8) == 0xF0) {  // 4-byte character
            codepoint = (c & 0x07) << 18;
            codepoint |= (utf8[i++] & 0x3F) << 12;
            codepoint |= (utf8[i++] & 0x3F) << 6;
            codepoint |= (utf8[i++] & 0x3F);
        }
        else {
            throw std::runtime_error("Invalid UTF-8");
        }
        utf32.push_back(codepoint);
    }
    return utf32;
}