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
    'src/ShaderBinding.cpp',
    'src/ShaderModule.cpp',
    'src/Texture.cpp',
    'src/Window.cpp',
]

vulkan = dependency('vulkan')
glm = dependency('glm', version : '>=0.9')
glfw = dependency('glfw3', version : '>=3.1')

engine_inc = include_directories(
    'src',
    'contrib/VMA/src',
    'contrib/stb'
)
engine_lib = library(
    'engine',
    engine_sources,
    include_directories : engine_inc,
    dependencies : [
        vulkan,
        glm,
        glfw
    ]
)
engine_dep = declare_dependency(
    link_with : engine_lib,
    include_directories : engine_inc
)
