#include <Window.h>
#include <Instance.h>

int main() {
    std::unique_ptr window = std::make_unique<vixen::Window>("Vixen Engine Test Application", false, 970, 680);
    std::unique_ptr instance = std::make_unique<vixen::Instance>("Vixen Engine Test Application", glm::ivec3(1, 0, 0));

    while (!window->shouldClose()) {
        window->update();
        
        window->swap();
    }
    
    return EXIT_SUCCESS;
}