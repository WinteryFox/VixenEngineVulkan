find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)
find_package(GLFW3 3.3 REQUIRED)
find_package(assimp REQUIRED)
pkg_check_modules(SPDLOG REQUIRED IMPORTED_TARGET spdlog)

add_custom_target(
        vert glslangValidator -V test.vert -o ${CMAKE_BINARY_DIR}/bin/vert.spv
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/shaders
)
add_custom_target(
        frag glslangValidator -V test.frag -o ${CMAKE_BINARY_DIR}/bin/frag.spv
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/shaders
)

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
add_dependencies(engine vert frag)
target_link_libraries(
        engine
        Vulkan::Vulkan
        glm::glm
        glfw
        assimp::assimp
        PkgConfig::SPDLOG
)
target_include_directories(
        engine PUBLIC
        contrib/VMA/src
        contrib/stb
)
