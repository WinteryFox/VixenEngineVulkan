#define VIXEN_DEBUG

#include <Window.h>
#include <Device.h>
#include <stdexcept>
#include <iostream>

int main() {
    vixen::Window* window = new vixen::Window("VixenEngine Test", false, 970, 680);
    
    while (!window->shouldClose()) {
        window->update();
        
        window->swap();
    }
    
    return EXIT_SUCCESS;
}