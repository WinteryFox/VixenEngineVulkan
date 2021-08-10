engine_sources = [
    'src/vk/DescriptorPool.cpp',
    'src/vk/DescriptorSetLayout.cpp',
    'src/vk/Framebuffer.cpp',
    'src/vk/Input.cpp',
    'src/vk/Instance.cpp',
    'src/vk/LogicalDevice.cpp',
    'src/vk/Mesh.cpp',
    'src/vk/PhysicalDevice.cpp',
    'src/vk/Render.cpp',
    'src/vk/Shader.cpp',
    'src/vk/ShaderDescriptor.cpp',
    'src/vk/ShaderModule.cpp',
    'src/vk/Window.cpp',
    'src/vk/Vulkan.cpp',
    'src/vk/Buffer.cpp',
    'src/vk/Logger.cpp',
    'src/vk/Image.cpp',
    'src/vk/ImageView.cpp',
    'src/vk/CommandBuffer.cpp',
    'src/vk/Fence.cpp',
    'src/vk/ImageSampler.cpp',
]

engine_deps = [
    dependency('threads'),
    dependency('vulkan'),
    dependency('glew', version : '>=2.2'),
    dependency('glm', version : '>=0.9'),
    dependency('glfw3', version : '>=3.1'),
    dependency('assimp', version : '>=5.0'),
    dependency('spdlog', version : '>=1.9.0')
]
engine_inc = include_directories(
    'src',
    'contrib/VMA/src',
    'contrib/stb'
)
engine_lib = library(
    'VixenEngine',
    engine_sources,
    include_directories : engine_inc,
    dependencies : engine_deps
)
engine = declare_dependency(
    link_with : engine_lib,
    include_directories : engine_inc,
    dependencies : engine_deps
)
