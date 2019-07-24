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
#include <Render.h>

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

    std::shared_ptr<vixen::Window> window(new vixen::Window("Vixen Engine Test Application"));
    std::shared_ptr<vixen::Instance> instance(
            new vixen::Instance(*window, "Vixen Engine Test Application", glm::ivec3(0, 0, 1), extensions, layers));
    std::shared_ptr<vixen::PhysicalDevice> physicalDevice(new vixen::PhysicalDevice(instance));
    std::shared_ptr<vixen::LogicalDevice> logicalDevice(new vixen::LogicalDevice(instance, window, physicalDevice));

    std::shared_ptr<vixen::Shader> vertex(new vixen::Shader(logicalDevice, "vert.spv"));
    std::shared_ptr<vixen::Shader> fragment(new vixen::Shader(logicalDevice, "frag.spv"));

    std::unique_ptr<vixen::Render> render(new vixen::Render(logicalDevice, physicalDevice, vertex, fragment, 3));

    while (!window->shouldClose()) {
        window->update();

        render->render();

        window->swap();
    }

    return EXIT_SUCCESS;
}