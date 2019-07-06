#pragma once

#ifdef __WIN32__

#include <windows.h>

#endif

#include <io.h>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <iostream>

namespace vixen {
    class Logger {
    public:
        Logger() {
            mkdir("logs/");
            time_t time = std::time(nullptr);
            std::ostringstream name;
            name << "logs/" << std::put_time(std::localtime(&time), "%Y%m%d%H%M%S") << ".log";
            stream.open(name.str().c_str(), std::ofstream::out | std::ofstream::app);
        }

        ~Logger() {
            stream.flush();
            stream.close();
        }

        void trace(const std::string &message) {
            log(SEVERITY::TRACE, message);
        }

        void info(const std::string &message) {
            log(SEVERITY::INFO, message);
        }

        void warning(const std::string &message) {
            log(SEVERITY::WARNING, message);
        }

        void error(const std::string &message) {
            log(SEVERITY::ERR, message);
        }

        void fatal(const std::string &message) {
            log(SEVERITY::FATAL, message);
#ifdef __WIN32__
            MessageBox(
                    nullptr,
                    message.c_str(),
                    "Fatal error occurred",
                    MB_ICONERROR | MB_OK | MB_DEFBUTTON2
            );
#endif
            exit(EXIT_FAILURE);
        }

    private:
        std::ofstream stream;

        enum class SEVERITY : int {
            TRACE = 5,
            INFO = 4,
            WARNING = 3,
            ERR = 2,
            FATAL = 1,
            NONE = 0
        };

        void log(SEVERITY severity, const std::string &message) {
            if (VIXEN_DEBUG_LEVEL < (int) severity)
                return;

            time_t time = std::time(nullptr);
            const auto &stamp = std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
            std::cout << stamp;
            stream << stamp;
            switch (severity) {
                case SEVERITY::TRACE:
                    std::cout << " [TRACE] ";
                    stream << " [TRACE] ";
                    break;
                case SEVERITY::INFO:
                    std::cout << " [INFO] ";
                    stream << " [INFO] ";
                    break;
                case SEVERITY::WARNING:
                    std::cerr << " [WARNING] ";
                    stream << " [WARNING] ";
                    break;
                case SEVERITY::ERR:
                    std::cerr << " [ERROR] ";
                    stream << " [ERROR] ";
                    break;
                case SEVERITY::FATAL:
                    std::cerr << " [FATAL] ";
                    stream << " [FATAL] ";
                case SEVERITY::NONE:
                    break;
            }

            std::cout << message << std::endl;
            stream << message << std::endl;
        }
    };
}