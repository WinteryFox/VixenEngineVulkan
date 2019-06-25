#pragma once

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <io.h>
#include <ctime>

#define DEBUG_LEVEL 4

namespace vixen {
    class Logger {
    public:
        Logger() {
            if (mkdir("./logs") != 0 && errno != EEXIST)
                throw std::runtime_error("Failed to create logs directory");

            stream.open("./logs/" + std::to_string(std::time(nullptr)), std::ofstream::out | std::ofstream::app);
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

            switch (severity) {
                case TRACE:
                    std::cout << glfwGetTime() << " [TRACE] " << message << std::endl;
                    stream << glfwGetTime() << " [TRACE] " << message << std::endl;
                    break;
                case INFO:
                    std::cout << glfwGetTime() << " [INFO] " << message << std::endl;
                    stream << glfwGetTime() << " [INFO] " << message << std::endl;
                    break;
                case WARNING:
                    std::cerr << glfwGetTime() << " [WARNING] " << message << std::endl;
                    stream << glfwGetTime() << " [WARNING] " << message << std::endl;
                    break;
                case ERROR:
                    std::cerr << glfwGetTime() << " [ERROR] " << message << std::endl;
                    stream << glfwGetTime() << " [ERROR] " << message << std::endl;
                    break;
            }
        }
    };
}