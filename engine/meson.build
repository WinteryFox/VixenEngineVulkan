engine_sources = [
    'src/DescriptorPool.cpp',
    'src/DescriptorSetLayout.cpp',
    'src/Framebuffer.cpp',
    'src/Input.cpp',
    'src/Instance.cpp',
    'src/LogicalDevice.cpp',
    'src/Mesh.cpp',
    'src/PhysicalDevice.cpp',
    'src/Render.cpp',
    'src/Shader.cpp',
    'src/ShaderDescriptor.cpp',
    'src/ShaderModule.cpp',
    'src/Window.cpp',
    'src/Vulkan.cpp',
    'src/Buffer.cpp',
    'src/Logger.cpp',
    'src/Image.cpp',
    'src/ImageView.cpp',
    'src/CommandBuffer.cpp',
    'src/Fence.cpp',
]

engine_deps = [
    dependency('vulkan'),
    dependency('glm', version : '>=0.9'),
    dependency('glfw3', version : '>=3.1'),
    dependency('assimp', version : '>=5.0'),
    dependency('spdlog', version : '>=1.9.0'),
    dependency('threads')
]
engine_inc = include_directories(
    'src',
    'contrib/VMA/src',
    'contrib/stb'
)
engine_lib = library(
    'engine',
    engine_sources,
    include_directories : engine_inc,
    dependencies : engine_deps
)
engine_dep = declare_dependency(
    link_with : engine_lib,
    include_directories : engine_inc,
    dependencies : engine_deps
)
