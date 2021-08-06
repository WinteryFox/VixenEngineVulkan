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
#include <VixenEngine.h>
#include "ModelViewProjection.h"

int main() {
    spdlog::set_level(spdlog::level::trace);

    const auto window = std::make_shared<Vixen::Window>("Vixen Engine Test Application", "../../icon.png");
    const auto instance = std::make_shared<Vixen::Instance>(window, "Vixen Engine Test Application",
                                                            glm::ivec3(VIXEN_TEST_VERSION_MAJOR,
                                                                       VIXEN_TEST_VERSION_MINOR,
                                                                       VIXEN_TEST_VERSION_PATCH));
    const auto physicalDevice = std::make_shared<Vixen::PhysicalDevice>(instance);
    const auto logicalDevice = std::make_shared<Vixen::LogicalDevice>(instance, window, physicalDevice);

    std::unique_ptr<Vixen::Camera> camera(new Vixen::Camera(
            {0, 0, 3}
    ));

    std::unique_ptr<Vixen::Input> input(new Vixen::Input(window));

    const auto meshStore = std::make_unique<Vixen::MeshStore>(logicalDevice, physicalDevice);
    meshStore->loadMesh("../../test/models/fox/Fox.fbx");
    meshStore->loadMesh("../../test/models/crystal/Crystal.fbx");

    Vixen::Scene scene{};
    //scene.entities.push_back(Vixen::Entity(meshStore->meshes[0], {}, {}, 0.01f));
    scene.entities.push_back(Vixen::Entity(meshStore->meshes[1], {}, {}, 0.01f));
    scene.entities.push_back(Vixen::Entity(meshStore->meshes[2], {}, {}, 1.0f));

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
                            .addAttribute(2, 2, VK_FORMAT_R32G32B32A32_SFLOAT, 0)
                            .addBinding(0, VK_VERTEX_INPUT_RATE_VERTEX, sizeof(glm::vec3))
                            .addBinding(1, VK_VERTEX_INPUT_RATE_VERTEX, sizeof(glm::vec2))
                            .addBinding(2, VK_VERTEX_INPUT_RATE_VERTEX, sizeof(glm::vec4))
                            .addDescriptor(0, sizeof(Vixen::Test::ModelViewProjection),
                                           VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
                            .addDescriptor(1, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                           VK_SHADER_STAGE_FRAGMENT_BIT)
                            .build())));

    Vixen::Logger logger = Vixen::Logger("Test");

    int fps = 0;
    double lastTime = 0;
    while (!window->shouldClose()) {
        window->update();

        input->update(camera);
        render->render(camera);

        double currentTime = glfwGetTime();
        fps++;
        if (currentTime - lastTime >= 1.0) {
            logger.info("FPS: {}", fps);
            fps = 0;
            lastTime = currentTime;
        }
    }

    return EXIT_SUCCESS;
}