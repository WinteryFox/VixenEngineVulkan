#pragma once

#ifdef __WIN32__

#include <windows.h>
#include <direct.h>

#elif __linux__

#include <sys/stat.h>

#endif

#include <ctime>
#include <iomanip>
#include <fstream>
#include <iostream>

// TODO: Redo this in a way that doesn't suck
namespace Vixen {
    enum class LogSeverity {
        LOG_SEVERITY_TRACE = 5,
        LOG_SEVERITY_INFO = 4,
        LOG_SEVERITY_WARNING = 3,
        LOG_SEVERITY_ERR = 2,
        LOG_SEVERITY_FATAL = 1,
        LOG_SEVERITY_NONE = 0
    };

    static void log(LogSeverity severity, const std::string &message) {
        //if (VIXEN_DEBUG_LEVEL < (int) severity)
        //    return;

        time_t time = std::time(nullptr);
        std::tm *localtime = std::localtime(&time);
        const auto &stamp = std::put_time(localtime, "%Y-%m-%d %H:%M:%S");

        std::stringstream out;
        out << stamp;
        switch (severity) {
            case LogSeverity::LOG_SEVERITY_TRACE:
                out << " [TRACE] ";
                break;
            case LogSeverity::LOG_SEVERITY_INFO:
                out << " [INFO] ";
                break;
            case LogSeverity::LOG_SEVERITY_WARNING:
                out << " [WARNING] ";
                break;
            case LogSeverity::LOG_SEVERITY_ERR:
                out << " [ERROR] ";
                break;
            case LogSeverity::LOG_SEVERITY_FATAL:
                out << " [FATAL] ";
            case LogSeverity::LOG_SEVERITY_NONE:
                break;
        }

        out << message << std::endl;
        std::cout << out.str();
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
        throw std::runtime_error(message);
    }

    static void error(const std::runtime_error &error) {
        log(LogSeverity::LOG_SEVERITY_ERR, error.what());
        throw error;
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