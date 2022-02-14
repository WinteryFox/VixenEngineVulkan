#pragma once

#define SPDLOG_FMT_EXTERNAL
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Vixen {
    class Logger {
        std::string name;

        std::shared_ptr<spdlog::logger> logger;

    public:
        explicit Logger(const std::string &name);

        ~Logger();

        template<typename... Args>
        void trace(fmt::format_string<Args...> message, Args &&...args) const {
            logger->log(spdlog::level::trace, message, std::forward<Args>(args)...);
        }

        template<typename... Args>
        void debug(fmt::format_string<Args...> message, Args &&...args) const {
            logger->log(spdlog::level::debug, message, std::forward<Args>(args)...);
        }

        template<typename... Args>
        void info(fmt::format_string<Args...> message, Args &&...args) const {
            logger->log(spdlog::level::info, message, std::forward<Args>(args)...);
        }

        template<typename... Args>
        void warning(fmt::format_string<Args...> message, Args &&...args) const {
            logger->log(spdlog::level::warn, message, std::forward<Args>(args)...);
        }

        template<typename... Args>
        void error(fmt::format_string<Args...> message, Args &&...args) const {
            logger->log(spdlog::level::err, message, std::forward<Args>(args)...);
        }

        template<typename... Args>
        void error(const std::runtime_error &error, Args &&...args) const {
            logger->log(spdlog::level::err, error.what(), std::forward<Args>(args)...);
        }

        template<typename... Args>
        void critical(fmt::format_string<Args...> message, Args &&...args) const {
            logger->log(spdlog::level::critical, message, std::forward<Args>(args)...);
        }
    };
}
