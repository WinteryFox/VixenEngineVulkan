#pragma once

#ifdef __WIN32__

#include <windows.h>
#include <direct.h>

#endif

#ifdef __linux__

#include <sys/stat.h>

#endif

#include <ctime>
#include <iomanip>
#include <fstream>
#include <iostream>

namespace Vixen {
    static const std::string LOG_DIR = "logs/";

    namespace {
        std::string initLogger() {
#ifdef __WIN32__
            mkdir(LOG_DIR.c_str());
#else
            mkdir(LOG_DIR.c_str(), 600);
#endif
            time_t time = std::time(nullptr);
            std::tm *localtime = std::localtime(&time);
            const auto &stamp = std::put_time(localtime, "%Y%m%d%H%M%S");
            std::stringstream stream;
            stream << stamp << ".log";
            return stream.str();
        }
    }

    static const std::string LOG_FILE = initLogger();

    enum class LogSeverity {
        LOG_SEVERITY_TRACE = 5,
        LOG_SEVERITY_INFO = 4,
        LOG_SEVERITY_WARNING = 3,
        LOG_SEVERITY_ERR = 2,
        LOG_SEVERITY_FATAL = 1,
        LOG_SEVERITY_NONE = 0
    };

    static void log(LogSeverity severity, const std::string &message) {
        if (VIXEN_DEBUG_LEVEL < (int) severity)
            return;

        time_t time = std::time(nullptr);
        std::tm *localtime = std::localtime(&time);
        const auto &stamp = std::put_time(localtime, "%Y-%m-%d %H:%M:%S");

        std::ofstream stream(LOG_DIR + LOG_FILE, std::ofstream::out | std::ofstream::app);

        std::cout << stamp;
        stream << stamp;
        switch (severity) {
            case LogSeverity::LOG_SEVERITY_TRACE:
                std::cout << " [TRACE] ";
                stream << " [TRACE] ";
                break;
            case LogSeverity::LOG_SEVERITY_INFO:
                std::cout << " [INFO] ";
                stream << " [INFO] ";
                break;
            case LogSeverity::LOG_SEVERITY_WARNING:
                std::cerr << " [WARNING] ";
                stream << " [WARNING] ";
                break;
            case LogSeverity::LOG_SEVERITY_ERR:
                std::cerr << " [ERROR] ";
                stream << " [ERROR] ";
                break;
            case LogSeverity::LOG_SEVERITY_FATAL:
                std::cerr << " [FATAL] ";
                stream << " [FATAL] ";
            case LogSeverity::LOG_SEVERITY_NONE:
                break;
        }

        std::cout << message << std::endl;
        stream << message << std::endl;

        stream.flush();
        stream.close();
    }

    static void trace(const std::string &message) {
        log(LogSeverity::LOG_SEVERITY_TRACE, message);
    }

    static void info(const std::string &message) {
        log(LogSeverity::LOG_SEVERITY_INFO, message);
    }

    static void warning(const std::string &message) {
        log(LogSeverity::LOG_SEVERITY_WARNING, message);
    }

    static void error(const std::string &message) {
        log(LogSeverity::LOG_SEVERITY_ERR, message);
    }

    static void fatal(const std::string &message) {
        log(LogSeverity::LOG_SEVERITY_FATAL, message);
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
}