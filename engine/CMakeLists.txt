find_package(PkgConfig REQUIRED)

#[[pkg_check_modules(VULKAN REQUIRED IMPORTED_TARGET vulkan)]]
find_package(Vulkan REQUIRED)
pkg_check_modules(GLM REQUIRED IMPORTED_TARGET glm)
pkg_check_modules(GLFW REQUIRED IMPORTED_TARGET glfw3)
pkg_check_modules(ASSIMP REQUIRED IMPORTED_TARGET assimp)
pkg_check_modules(SPDLOG REQUIRED IMPORTED_TARGET spdlog)

add_library(
        engine SHARED
        src/DescriptorPool.cpp
        src/DescriptorSetLayout.cpp
        src/Framebuffer.cpp
        src/Input.cpp
        src/Instance.cpp
        src/LogicalDevice.cpp
        src/Mesh.cpp
        src/PhysicalDevice.cpp
        src/Render.cpp
        src/Shader.cpp
        src/ShaderDescriptor.cpp
        src/ShaderModule.cpp
        src/Window.cpp
        src/Vulkan.cpp
        src/Buffer.cpp
        src/Logger.cpp
        src/Image.cpp
        src/ImageView.cpp
        src/CommandBuffer.cpp
        src/Fence.cpp
        src/ImageSampler.cpp
)
target_link_libraries(
        engine
        Vulkan::Vulkan
        PkgConfig::GLM
        PkgConfig::GLFW
        PkgConfig::ASSIMP
        PkgConfig::SPDLOG
)
