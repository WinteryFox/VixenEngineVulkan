#include <Window.h>
#include <Instance.h>
#include <Logger.h>

int main() {
    std::vector<const char *> extensions = {
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME
    };
    std::vector<const char *> layers = {
            "VK_LAYER_KHRONOS_validation"
    };

    std::unique_ptr window = std::make_unique<vixen::Window>(
            "Vixen Engine Test Application",
            false,
            970,
            680
    );
    std::unique_ptr instance = std::make_unique<vixen::Instance>(
            "Vixen Engine Test Application",
            glm::ivec3(0, 0, 1),
            extensions,
            layers
    );
    vixen::Logger logger = vixen::Logger();

    while (!window->shouldClose()) {
        window->update();

        window->swap();
    }

    return EXIT_SUCCESS;
}