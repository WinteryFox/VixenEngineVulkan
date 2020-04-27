#pragma once

#include <stdexcept>
#include <utility>

namespace Vixen {
    struct IOException : public std::runtime_error {
        const std::string path;

        IOException(const std::string &message, std::string path) : std::runtime_error(message), path(std::move(path)) {}
    };
}