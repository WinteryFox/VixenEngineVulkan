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

#include <memory>
#include <Window.h>
#include <Instance.h>
#include <Logger.h>
#include <PhysicalDevice.h>
#include <LogicalDevice.h>
#include <Shader.h>

int main() {
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
    std::shared_ptr instance = std::make_shared<vixen::Instance>(*window, "Vixen Engine Test Application",
                                                                 glm::ivec3(0, 0, 1),
                                                                 extensions, layers);
    std::unique_ptr physicalDevice = std::make_unique<vixen::PhysicalDevice>(*instance);
    std::unique_ptr logicalDevice = std::make_unique<vixen::LogicalDevice>(*instance, *physicalDevice);

    vixen::Shader vert(logicalDevice.get(), "vert.spv");
    vixen::Shader frag(logicalDevice.get(), "frag.spv");

    while (!window->shouldClose()) {
        window->update();

        window->swap();
    }

    return EXIT_SUCCESS;
}