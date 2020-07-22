#pragma once

namespace Vixen {
    inline constexpr double PI = 3.14159265358979323846;

    inline constexpr double radiansToDegrees(const double radians) {
        return radians * (180.0 / PI);
    }

    inline constexpr glm::vec3 radiansToDegrees(const glm::vec3 &degrees) {
        return {
                radiansToDegrees(degrees.x),
                radiansToDegrees(degrees.y),
                radiansToDegrees(degrees.z)
        };
    }

    inline constexpr double degreesToRadians(const double degrees) {
        return degrees * (PI / 180.0);
    }

    inline constexpr glm::vec3 degreesToRadians(const glm::vec3 &degrees) {
        return {
            degreesToRadians(degrees.x),
            degreesToRadians(degrees.y),
            degreesToRadians(degrees.z)
        };
    }
}