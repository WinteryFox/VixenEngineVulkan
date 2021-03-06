/**
 * This is a simple test executable to test new or experimental functionality.
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
            "VK_LAYER_KHRONOS_validation"
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
            {0, 0, 3}
    ));

    std::unique_ptr<Vixen::Input> input(new Vixen::Input(window));

    std::unique_ptr<Vixen::MeshStore> meshStore(new Vixen::MeshStore(logicalDevice, physicalDevice));
    meshStore->loadMesh("../../test/models/Fox.FBX");
    meshStore->loadMesh("../../test/models/Crystal.FBX");

    Vixen::Scene scene{};
    scene.entities.push_back(Vixen::Entity(meshStore->meshes[1], {}, {}, 0.01f));
    scene.entities.push_back(Vixen::Entity(meshStore->meshes[2], {0.0, 0.0, 10.0}, {}, 1.0f));

    std::unique_ptr<Vixen::Render> render(new Vixen::Render(
            logicalDevice,
            physicalDevice,
            scene,
            std::shared_ptr<Vixen::Shader>(
                    Vixen::Shader::Builder()
                            .addModule(Vixen::ShaderModule::Builder(logicalDevice)
                                               .setShaderStage(VK_SHADER_STAGE_VERTEX_BIT)
                                               .setBytecode("vert.spv")
                                               .build())
                            .addModule(Vixen::ShaderModule::Builder(logicalDevice)
                                               .setShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT)
                                               .setBytecode("frag.spv")
                                               .build())
                            .addAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0)
                            .addAttribute(1, 1, VK_FORMAT_R32G32_SFLOAT, 0)
                            .addBinding(3 * sizeof(glm::mat4), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0,
                                                   VK_VERTEX_INPUT_RATE_VERTEX, sizeof(glm::vec3))
                            .addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                   VK_SHADER_STAGE_FRAGMENT_BIT, 1, VK_VERTEX_INPUT_RATE_VERTEX, sizeof(glm::vec2))
                            .build())));

    int fps = 0;
    double lastTime = 0;

    while (!window->shouldClose()) {
        window->update();

        input->update(camera);
        render->render(camera);

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