# ---- BUILD ENGINE ----

find_package(Threads REQUIRED)

set(FBX_IMPORT_BUILD_TEST OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/contrib/FBXImport)

find_package(PkgConfig REQUIRED)
pkg_check_modules(VIXEN_LIBS REQUIRED IMPORTED_TARGET glfw3>=3.3 vulkan>=1.2 glm)

set(
        vixen_HEADERS
        include/Window.h
        include/Instance.h
        include/Logger.h
        include/PhysicalDevice.h
        include/LogicalDevice.h
        include/ShaderModule.h
        include/Render.h
        include/Mesh.h
        include/Util.h
        include/VixenEngine.h
        include/Entity.h
        include/Scene.h
        include/Camera.h
        include/Input.h
        include/MeshStore.h
        include/Texture.h
        include/exceptions/VulkanException.h
        include/exceptions/ImageException.h
        include/exceptions/IOException.h
        include/SimpleMath.h
        include/Framebuffer.h
        include/Shader.h
        include/ShaderBinding.h
        include/DescriptorSetLayout.h
        include/DescriptorPool.h)
set(
        vixen_SOURCES
        properties.rc
        src/Window.cpp
        src/Instance.cpp
        src/PhysicalDevice.cpp
        src/LogicalDevice.cpp
        src/ShaderModule.cpp
        src/Render.cpp
        src/Mesh.cpp
        src/Input.cpp
        src/Texture.cpp
        src/Framebuffer.cpp
        src/Shader.cpp
        src/ShaderBinding.cpp
        src/DescriptorSetLayout.cpp
        src/DescriptorPool.cpp)

add_library(
        VixenEngine
        SHARED
        ${vixen_SOURCES}
        ${vixen_HEADERS}
)

target_include_directories(
        VixenEngine
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${Vulkan_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/contrib/FBXImport/importer/include
        ${CMAKE_CURRENT_SOURCE_DIR}/contrib/stb
        ${CMAKE_CURRENT_SOURCE_DIR}/contrib/VMA/src
        ${CMAKE_CURRENT_SOURCE_DIR}/contrib/range-v3/include
        ${GLM_INCLUDEDIR}
)

target_link_libraries(
        VixenEngine
        Threads::Threads
        PkgConfig::VIXEN_LIBS
        FBXImport
)

set_target_properties(
        VixenEngine PROPERTIES
        PREFIX ""
        OUTPUT_NAME "VixenEngine"
)