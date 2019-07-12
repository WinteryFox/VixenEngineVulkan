/**
 * This is a simple test executionable to test new or experimental functionality.
 * Do not use this in your project.
 *
 * @author Foxie
 * @date 7/7/2019
 */

#ifdef __WIN32__

#include <windows.h>

#endif

#include <Window.h>
#include <Instance.h>
#include <Logger.h>
#include <PhysicalDevice.h>
#include <LogicalDevice.h>

int main() {
#ifdef __WIN32__
#ifndef VIXEN_DEBUG
    FreeConsole();
#endif
#endif

    std::vector<const char *> extensions = {
#ifdef VIXEN_DEBUG
            "VK_EXT_debug_utils",
#endif
            "VK_KHR_get_physical_device_properties2"
    };

    std::vector<const char *> layers = {
#ifdef VIXEN_DEBUG
            "VK_LAYER_LUNARG_standard_validation"
#endif
    };

    std::unique_ptr window = std::make_unique<vixen::Window>("Vixen Engine Test Application");
    std::unique_ptr instance = std::make_unique<vixen::Instance>(*window, "Vixen Engine Test Application",
                                                                 glm::ivec3(0, 0, 1),
                                                                 extensions, layers);
    std::unique_ptr physicalDevice = std::make_unique<vixen::PhysicalDevice>(*instance);
    std::unique_ptr logicalDevice = std::make_unique<vixen::LogicalDevice>(*physicalDevice);

    while (!window->shouldClose()) {
        window->update();

        window->swap();
    }

    return EXIT_SUCCESS;
}