/**
 * This is a simple test executionable to test new or experimental functionality.
 * Do not use this in your project.
 *
 * @author Foxie
 * @date 7/7/2019
 */

#include <memory>
#include <vector>
#include <VixenEngine.h>

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

    std::unique_ptr<vixen::Shader> vertex(new vixen::Shader(logicalDevice, "vert.spv"));
    std::unique_ptr<vixen::Shader> fragment(new vixen::Shader(logicalDevice, "frag.spv"));

    std::vector<glm::vec3> vertices = {
            {-0.5f, -0.5f, 0.0f},
            {0.5f,  -0.5f, 0.0f},
            {0.5f,  0.5f,  0.0f},
            {-0.5f, 0.5f,  0.0f},
    };

    std::vector<uint32_t> indices = {
            0, 1, 2, 2, 3, 0
    };

    std::shared_ptr<vixen::Mesh> mesh(new vixen::Mesh(logicalDevice, vertices, indices));

    vixen::Scene scene = {};
    scene.entities.emplace_back(mesh);
    std::unique_ptr<vixen::Render> render(new vixen::Render(logicalDevice, physicalDevice, scene, vertex, fragment, 3));

    int fps = 0;
    double lastTime = 0;

    while (!window->shouldClose()) {
        window->update();

        render->render();

        window->swap();

        double currentTime = glfwGetTime();
        fps++;
        if (currentTime - lastTime >= 1.0) {
            vixen::info("FPS: " + std::to_string(fps));
            fps = 0;
            lastTime = currentTime;
        }
    }

    return EXIT_SUCCESS;
}