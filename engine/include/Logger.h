#pragma once

#ifdef __WIN32__

#include <windows.h>
#include <direct.h>

#endif

#include <ctime>
#include <iomanip>
#include <fstream>
#include <iostream>

namespace vixen {
    enum class SEVERITY : int {
        TRACE = 5,
        INFO = 4,
        WARNING = 3,
        ERR = 2,
        FATAL = 1,
        NONE = 0
    };

    static void log(SEVERITY severity, const std::string &message) {
        if (VIXEN_DEBUG_LEVEL < (int) severity)
            return;

#ifdef __WIN32__
        mkdir("logs/");
#else
        mkdir("logs/", 0777)
#endif
        time_t time = std::time(nullptr);

        std::ostringstream name;
        std::tm *localtime = std::localtime(&time);
        const auto &stampName = std::put_time(localtime, "%Y%m%d%H%M%S");
        name << "logs/" << stampName << ".log";

        const auto &stamp = std::put_time(localtime, "%Y-%m-%d %H:%M:%S");
        std::ofstream stream(name.str().c_str(), std::ofstream::out | std::ofstream::app);

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

        stream.flush();
        stream.close();
    }

    static void trace(const std::string &message) {
        log(SEVERITY::TRACE, message);
    }

    static void info(const std::string &message) {
        log(SEVERITY::INFO, message);
    }

    static void warning(const std::string &message) {
        log(SEVERITY::WARNING, message);
    }

    static void error(const std::string &message) {
        log(SEVERITY::ERR, message);
    }

    static void fatal(const std::string &message) {
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
}