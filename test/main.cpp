#include <Window.h>
#include <Instance.h>
#include <Logger.h>
#include <PhysicalDevice.h>

#ifdef __WIN32__

#include <windows.h>

#endif

int main() {
#ifdef __WIN32__
#ifndef VIXEN_DEBUG
    FreeConsole();
#endif
#endif

    std::vector<const char *> extensions = {
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
            "VK_KHR_get_physical_device_properties2"
    };

    std::vector<const char *> layers = {
#ifdef VIXEN_DEBUG
            "VK_LAYER_KHRONOS_validation",
            "VK_LAYER_LUNARG_parameter_validation",
            "VK_LAYER_LUNARG_core_validation",
            "VK_LAYER_LUNARG_object_tracker"
#endif
    };

    std::unique_ptr window = std::make_unique<vixen::Window>(
            "Vixen Engine Test Application"
    );
    std::unique_ptr instance = std::make_unique<vixen::Instance>(
            "Vixen Engine Test Application",
            glm::ivec3(0, 0, 1),
            extensions,
            layers
    );
    std::unique_ptr physicalDevice = std::make_unique<vixen::PhysicalDevice>(
            *instance
    );
    while (!window->shouldClose()) {
        window->update();

        window->swap();
    }

    return EXIT_SUCCESS;
}