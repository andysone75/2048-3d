#include "Utils.h"
#include <iostream>

namespace utils {
    Vector3 lerp(Vector3 a, Vector3 b, float t) {
        return Vector3{
            a.x + (b.x - a.x) * t,
            a.y + (b.y - a.y) * t,
            a.z + (b.z - a.z) * t
        };
    }
}