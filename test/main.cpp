/**
 * This is a simple test executionable to test new or experimental functionality.
 * Do not use this in your project.
 *
 * @author Foxie
 * @date 7/7/2019
 */

#include <memory>
#include <Window.h>
#include <Instance.h>
#include <PhysicalDevice.h>
#include <LogicalDevice.h>
#include <Shader.h>
#include <Render.h>
#include <Mesh.h>

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

    std::unique_ptr<vixen::Window> window(new vixen::Window("Vixen Engine Test Application"));
    std::unique_ptr<vixen::Instance> instance(
            new vixen::Instance(*window, "Vixen Engine Test Application", glm::ivec3(0, 0, 1), extensions, layers));
    std::unique_ptr<vixen::PhysicalDevice> physicalDevice(new vixen::PhysicalDevice(instance));
    std::unique_ptr<vixen::LogicalDevice> logicalDevice(new vixen::LogicalDevice(instance, window, physicalDevice));

    vixen::Shader vertex(logicalDevice, "vert.spv");
    vixen::Shader fragment(logicalDevice, "frag.spv");

    std::unique_ptr<vixen::Render> render(new vixen::Render(logicalDevice, physicalDevice, vertex, fragment, 3));

    std::vector<glm::vec3> vertices = {
            glm::vec3(-0.5f, -0.5f, 0.0f),
            glm::vec3(0.5f, 0.5f, 0.0f),
            glm::vec3(-0.5f, 0.5f, 0.0f),
            glm::vec3(-0.5f, -0.5f, 0.0f),
            glm::vec3(0.5f, -0.5f, 0.0f),
            glm::vec3(0.5f, 0.5f, 0.0f)
    };

    render->addMesh(std::make_unique<vixen::Mesh>(logicalDevice, physicalDevice, vertices));

    while (!window->shouldClose()) {
        window->update();

        render->render();

        window->swap();
    }

    return EXIT_SUCCESS;
}