# ---- BUILD ENGINE ----

set(FBX_IMPORT_BUILD_TEST OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/contrib/FBXImport)

find_package(PkgConfig REQUIRED)
pkg_check_modules(VIXEN_LIBS REQUIRED IMPORTED_TARGET glfw3>=3.3 vulkan>=1.2 glm)

set(
        vixen_LIBRARY
        Vulkan::Vulkan
        Threads::Threads
        FBXImport
        -static
)
set(
        vixen_INCLUDE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${Vulkan_INCLUDE_DIR}
        ${GLFW_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/contrib/FBXImport/importer/include
        ${CMAKE_CURRENT_SOURCE_DIR}/contrib/stb
        ${CMAKE_CURRENT_SOURCE_DIR}/contrib/glm
        ${CMAKE_CURRENT_SOURCE_DIR}/contrib/VMA/src
)

if (vixen_BUILD_GLFW)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/contrib/glfw)
    list(APPEND vixen_LIBRARY glfw)
    list(APPEND vixen_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/contrib/glfw/include)
else (vixen_BUILD_GLFW)
    find_package(GLFW REQUIRED)
    list(APPEND vixen_LIBRARY ${GLFW_LIBRARY})
    list(APPEND vixen_INCLUDE ${GLFW_INCLUDE})
endif (vixen_BUILD_GLFW)

set(
        vixen_HEADERS
        include/Window.h
        include/Instance.h
        include/Logger.h
        include/PhysicalDevice.h
        include/LogicalDevice.h
        include/Shader.h
        include/Render.h
        include/Mesh.h
        include/Loader.h
        include/VixenEngine.h
        include/Entity.h
        include/Scene.h
        include/Camera.h
        include/Input.h
        include/VertexShader.h
        include/FragmentShader.h
        include/MeshStore.h)
set(
        vixen_SOURCES
        properties.rc
        src/Window.cpp
        src/Instance.cpp
        src/PhysicalDevice.cpp
        src/LogicalDevice.cpp
        src/Shader.cpp
        src/Render.cpp
        src/Mesh.cpp
        src/Input.cpp
)

add_library(
        VixenEngine
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
        PkgConfig::VIXEN_LIBS
        FBXImport
)

set_target_properties(
        VixenEngine PROPERTIES
        PREFIX ""
        OUTPUT_NAME "VixenEngine"
)