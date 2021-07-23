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
glm = dependency('glm')
glfw = dependency('glfw3')

engine = shared_library(
    'engine',
    engine_sources,
    include_directories : [
        'include',
        'contrib/VMA/src',
        'contrib/stb'
    ],
    dependencies : [
        vulkan,
        glm,
        glfw
    ]
)
