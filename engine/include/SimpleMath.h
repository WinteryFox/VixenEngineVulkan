#pragma once

namespace Vixen {
    inline constexpr double PI = 3.14159265358979323846;

    inline constexpr double radiansToDegrees(double radians) {
        return radians * (180.0 / PI);
    }

    inline constexpr double degreesToRadians(double degrees) {
        return degrees * (PI / 180.0);
    }
}