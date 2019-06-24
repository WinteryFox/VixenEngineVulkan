#include "Instance.h"

namespace vixen {
    Instance::Instance(const std::string& appName, glm::ivec3 appVersion) {
        // The application info
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = appName.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(appVersion.x, appVersion.y, appVersion.z);
        appInfo.pEngineName = "Vixen Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
        appInfo.apiVersion = VK_API_VERSION_1_1;

        std::cout << "Vixen Engine version " << appInfo.engineVersion << " running Vulkan version 1.1" << std::endl;
        std::cout << "Serving application " << appInfo.pApplicationName << " " << appInfo.applicationVersion << std::endl;

        // Fetch extensions for instance info
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        // The instance info
        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        createInfo.enabledLayerCount = 0;

        // Attempt to create the instance, throws runtime error if no instance could be made
        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
            throw std::runtime_error("Failed to create Vulkan instance");
        std::cout << "Successfully created Vulkan instance!" << std::endl;

        // Query and store the supported extensions
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        extensions.resize(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
        std::cout << extensionCount << " extensions available; ";
        for (auto const& ext : extensions)
            std::cout << ext.extensionName << "(" << ext.specVersion << "), ";
        std::cout << std::endl;
    }
}