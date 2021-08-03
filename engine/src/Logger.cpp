#include "Logger.h"

namespace Vixen {
    Logger::Logger(const std::string &name) : name(name), logger(spdlog::stdout_color_mt(name)) {
        logger->set_pattern("%Y-%m-%d %T.%e %7l %P --- [%t] %20n : %v");
    }

    Logger::~Logger() {
        spdlog::drop(name);
    }
}
