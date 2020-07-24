/**
 * This is a simple test executionable to test new or experimental functionality.
 * Do not use this in your project.
 *
 * @author Foxie
 * @date 7/7/2019
 */

inline constexpr int VIXEN_TEST_VERSION_MAJOR = 0;
inline constexpr int VIXEN_TEST_VERSION_MINOR = 0;
inline constexpr int VIXEN_TEST_VERSION_PATCH = 1;

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

    std::unique_ptr<Vixen::Window> window(new Vixen::Window("Vixen Engine Test Application", "../../icon.png"));
    std::unique_ptr<Vixen::Instance> instance(
            new Vixen::Instance(*window, "Vixen Engine Test Application",
                                glm::ivec3(VIXEN_TEST_VERSION_MAJOR, VIXEN_TEST_VERSION_MINOR,
                                           VIXEN_TEST_VERSION_PATCH), extensions, layers));
    std::unique_ptr<Vixen::PhysicalDevice> physicalDevice(new Vixen::PhysicalDevice(instance));
    std::unique_ptr<Vixen::LogicalDevice> logicalDevice(new Vixen::LogicalDevice(instance, window, physicalDevice));

    std::unique_ptr<Vixen::Camera> camera(new Vixen::Camera(
            {0, 0, -3},
            {}
    ));

    std::unique_ptr<Vixen::Input> input(new Vixen::Input(window));

    std::unique_ptr<Vixen::MeshStore> meshStore(new Vixen::MeshStore(logicalDevice, physicalDevice));
    meshStore->loadMesh("../../test/models/Fox.FBX");
    meshStore->loadMesh("../../test/models/Tree.FBX");

    Vixen::Scene scene{};
    scene.entities.push_back(Vixen::Entity(meshStore->meshes[1], {}, {}, 0.01f));
    scene.entities.push_back(Vixen::Entity(meshStore->meshes[2], {0.0, 0.0, 10.0}, {}, 0.001f));

    std::unique_ptr<Vixen::Render> render(new Vixen::Render(
            logicalDevice,
            physicalDevice,
            scene,
            Vixen::Shader::Builder()
                    .addModule(
                            Vixen::ShaderModule::Builder(logicalDevice)
                                    .setShaderStage(VK_SHADER_STAGE_VERTEX_BIT)
                                    .setBytecode("vert.spv")
                                    .addAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0)
                                    .addAttribute(1, 1, VK_FORMAT_R32G32_SFLOAT, 0)
                                    .addBinding(0, VK_VERTEX_INPUT_RATE_VERTEX, 0)
                                    .addBinding(1, VK_VERTEX_INPUT_RATE_VERTEX, 0)
                                    .addBinding(2, VK_VERTEX_INPUT_RATE_VERTEX, 0)
                                    .build())
                    .addModule(
                            Vixen::ShaderModule::Builder(logicalDevice)
                                    .setShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT)
                                    .setBytecode("frag.spv")
                                    .addBinding(1, VK_VERTEX_INPUT_RATE_INSTANCE, 0)
                                    .build())
                    .build()
    ));

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