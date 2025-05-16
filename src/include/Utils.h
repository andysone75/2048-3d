#pragma once

#include <string>

namespace Utils {
	inline float lerp(float a, float b, float f) { return a + f * (b - a); }
	std::u32string utf8_to_utf32(const std::string& utf8);
}