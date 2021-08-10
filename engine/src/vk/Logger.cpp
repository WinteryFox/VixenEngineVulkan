#include "Logger.h"

namespace Vixen::Vk {
    Logger::Logger(const std::string &name) : name(name) {
        const auto &l = spdlog::get(name);
        if (!l) {
            logger = spdlog::stdout_color_mt(name);
            logger->set_pattern("%Y-%m-%d %T.%e %^%7l%$ %P --- [%t] %20n : %1v");
        } else {
            logger = l;
        }
    }

    Logger::~Logger() {
        spdlog::drop(name);
    }
}
