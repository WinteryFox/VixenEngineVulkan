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

    std::unique_ptr<Vixen::Window> window(new Vixen::Window("Vixen Engine Test Application"));
    std::unique_ptr<Vixen::Instance> instance(
            new Vixen::Instance(*window, "Vixen Engine Test Application", glm::ivec3(0, 0, 1), extensions, layers));
    std::unique_ptr<Vixen::PhysicalDevice> physicalDevice(new Vixen::PhysicalDevice(instance));
    std::unique_ptr<Vixen::LogicalDevice> logicalDevice(new Vixen::LogicalDevice(instance, window, physicalDevice));

    std::unique_ptr<Vixen::Shader> vertex(new Vixen::Shader(logicalDevice, Vixen::SHADER_TYPE_VERTEX, "vert.spv"));
    std::unique_ptr<Vixen::Shader> fragment(new Vixen::Shader(logicalDevice, Vixen::SHADER_TYPE_FRAGMENT, "frag.spv"));

    std::vector<glm::vec3> vertices = {
            {-0.5f, -0.5f, 0.0f},
            {0.5f,  -0.5f, 0.0f},
            {0.5f,  0.5f,  0.0f},
            {-0.5f, 0.5f,  0.0f},
    };

    std::vector<uint32_t> indices = {
            0, 1, 2, 2, 3, 0
    };

    std::shared_ptr<Vixen::Mesh> mesh(new Vixen::Mesh(logicalDevice, vertices, indices));

    Vixen::Scene scene = {};
    scene.entities.emplace_back(mesh);
    std::unique_ptr<Vixen::Render> render(new Vixen::Render(logicalDevice, physicalDevice, scene, vertex, fragment, 3));

    int fps = 0;
    double lastTime = 0;

    while (!window->shouldClose()) {
        window->update();

        render->render();

        window->swap();

        double currentTime = glfwGetTime();
        fps++;
        if (currentTime - lastTime >= 1.0) {
            Vixen::info("FPS: " + std::to_string(fps));
            fps = 0;
            lastTime = currentTime;
        }
    }

    return EXIT_SUCCESS;
}