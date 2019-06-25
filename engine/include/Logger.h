#pragma once

#include <fstream>
#include <ctime>
#include <iomanip>

#define DEBUG_LEVEL 4

namespace vixen {
    class Logger {
    public:
        Logger() {
            stream.open("latest.log", std::ofstream::out | std::ofstream::app);
        }

        ~Logger() {
            stream.flush();
            stream.close();
        }

        void trace(const std::string &message) {
            log(TRACE, message);
        }

        void info(const std::string &message) {
            log(INFO, message);
        }

        void warning(const std::string &message) {
            log(WARNING, message);
        }

        void error(const std::string &message) {
            log(ERROR, message);
        }

    private:
        std::ofstream stream;

        enum SEVERITY {
            TRACE = 4,
            INFO = 3,
            WARNING = 2,
            ERROR = 1,
            NONE = 0
        };

        void log(SEVERITY severity, const std::string &message) {
            if (DEBUG_LEVEL < severity)
                return;

            time_t time = std::time(nullptr);
            const auto &stamp = std::put_time(std::localtime(&time), "%c");
            std::cout << stamp;
            stream << stamp;
            switch (severity) {
                case TRACE:
                    std::cout << " [TRACE] ";
                    stream << " [TRACE] ";
                    break;
                case INFO:
                    std::cout << " [INFO] ";
                    stream << " [INFO] ";
                    break;
                case WARNING:
                    std::cerr << " [WARNING] ";
                    stream << " [WARNING] ";
                    break;
                case ERROR:
                    std::cerr << " [ERROR] ";
                    stream << " [ERROR] ";
                    break;
            }
            std::cout << message << std::endl;
            stream << message << std::endl;
        }
    };
}