/**
 * This is a simple test executionable to test new or experimental functionality.
 * Do not use this in your project.
 *
 * @author Foxie
 * @date 7/7/2019
 */

#define VIXEN_TEST_VERSION_MAJOR 0
#define VIXEN_TEST_VERSION_MINOR 0
#define VIXEN_TEST_VERSION_PATCH 1

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
            new Vixen::Instance(*window, "Vixen Engine Test Application",
                                glm::ivec3(VIXEN_TEST_VERSION_MAJOR, VIXEN_TEST_VERSION_MINOR,
                                           VIXEN_TEST_VERSION_PATCH), extensions, layers));
    std::unique_ptr<Vixen::PhysicalDevice> physicalDevice(new Vixen::PhysicalDevice(instance));
    std::unique_ptr<Vixen::LogicalDevice> logicalDevice(new Vixen::LogicalDevice(instance, window, physicalDevice));

    std::unique_ptr<Vixen::VertexShader> vertex(new Vixen::VertexShader(logicalDevice, "/home/winteryfox/Desktop/VixenEngineVulkan/test/shaders/vert.spv"));
    std::unique_ptr<Vixen::FragmentShader> fragment(new Vixen::FragmentShader(logicalDevice, "/home/winteryfox/Desktop/VixenEngineVulkan/test/shaders/frag.spv"));

    std::unique_ptr<Vixen::Camera> camera(new Vixen::Camera(
            {0.0f, 0.9f, 1.0f},
            {0.0f, 0.0f, -1.0f}
    ));

    std::unique_ptr<Vixen::Input> input(new Vixen::Input(window));

    std::shared_ptr<Vixen::Mesh> mesh(new Vixen::Mesh(logicalDevice, "/home/winteryfox/Desktop/fox.fbx"));
    Vixen::Scene scene = {};
    scene.entities.emplace_back(mesh);
    std::unique_ptr<Vixen::Render> render(new Vixen::Render(logicalDevice, physicalDevice, scene, vertex, fragment));

    int fps = 0;
    double lastTime = 0;

    while (!window->shouldClose()) {
        window->update();

        input->update(camera);
        render->render(camera);

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