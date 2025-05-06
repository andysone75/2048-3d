#pragma once

namespace Utils {
	inline float lerp(float a, float b, float f) { return a + f * (b - a); }
}