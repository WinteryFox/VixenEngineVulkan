#include <memory>
#include <VixenEngine.h>

inline constexpr int VIXEN_TEST_VERSION_MAJOR = 0;
inline constexpr int VIXEN_TEST_VERSION_MINOR = 0;
inline constexpr int VIXEN_TEST_VERSION_PATCH = 1;
inline constexpr const char* VIXEN_EDITOR_NAME = "Vixen Editor";

int main() {
    spdlog::set_level(spdlog::level::trace);
    Vixen::Logger logger = Vixen::Logger(VIXEN_EDITOR_NAME);

    const auto window = std::make_shared<Vixen::Window>(VIXEN_EDITOR_NAME, "../../icon.png");
    const auto instance = std::make_shared<Vixen::Instance>(window, VIXEN_EDITOR_NAME,
                                                            glm::ivec3(VIXEN_TEST_VERSION_MAJOR,
                                                                       VIXEN_TEST_VERSION_MINOR,
                                                                       VIXEN_TEST_VERSION_PATCH));
    const auto physicalDevice = std::make_shared<Vixen::PhysicalDevice>(instance);
    const auto logicalDevice = std::make_shared<Vixen::LogicalDevice>(instance, window, physicalDevice);

    std::unique_ptr<Vixen::Input> input(new Vixen::Input(window));

    const auto meshStore = std::make_unique<Vixen::MeshStore>(logicalDevice, physicalDevice);
    meshStore->loadMesh("../../editor/models/fox/Fox.fbx");
    meshStore->loadMesh("../../editor/models/crystal/Crystal.fbx");
    //meshStore->loadMesh("../../editor/models/michiru/Meshes/MichiruSkel_v001_002.fbx");
    meshStore->loadMesh("../../editor/models/ruby_rose/Mesh/rubySkel_v001_002.fbx");

    Vixen::Scene scene{};
    scene.camera.position = {0, 0, 3};
    // Fox
    //scene.entities.push_back(Vixen::Entity(meshStore->meshes[0], {}, {}, 0.01f));
    //scene.entities.push_back(Vixen::Entity(meshStore->meshes[1], {}, {}, 0.01f));

    // Crystal
    scene.entities.push_back(Vixen::Entity(meshStore->meshes[2], {}, {}, 1.0f));

    // Michiru
    //scene.entities.push_back(Vixen::Entity(meshStore->meshes[4], {}, {}, 0.01f));
    //scene.entities.push_back(Vixen::Entity(meshStore->meshes[5], {}, {}, 0.01f));

    // Ruby
    scene.entities.push_back(Vixen::Entity(meshStore->meshes[3], {}, {}, 0.01f));
    scene.entities.push_back(Vixen::Entity(meshStore->meshes[4], {}, {}, 0.01f));
    scene.entities.push_back(Vixen::Entity(meshStore->meshes[5], {}, {}, 0.01f));
    scene.entities.push_back(Vixen::Entity(meshStore->meshes[6], {}, {}, 0.01f));
    scene.entities.push_back(Vixen::Entity(meshStore->meshes[7], {}, {}, 0.01f));
    scene.entities.push_back(Vixen::Entity(meshStore->meshes[8], {}, {}, 0.01f));
    scene.entities.push_back(Vixen::Entity(meshStore->meshes[9], {}, {}, 0.01f));

    std::unique_ptr<Vixen::Render> render(new Vixen::Render(
            logicalDevice,
            physicalDevice,
            scene,
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
                    .addDescriptor(0, 3 * sizeof(glm::mat4), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
                    .addDescriptor(1, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                   VK_SHADER_STAGE_FRAGMENT_BIT)
                    .build()));

    int fps = 0;
    double lastTime = 0;
    while (!window->shouldClose()) {
        scene.entities[0].rotation.y += 5 * static_cast<float>(render->getDeltaTime());

        window->update();

        input->update(scene.camera, render->getDeltaTime());
        render->render(scene.camera);

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