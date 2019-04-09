#include <Window.h>

int main() {
    std::unique_ptr<vixen::Window> window = std::make_unique<vixen::Window>("VixenEngine Test", false, 970, 680);
    
    while (!window->shouldClose()) {
        window->update();
        
        window->swap();
    }
    
    return EXIT_SUCCESS;
}