validator = find_program('glslangValidator')
vertex = custom_target(
    'vertex_shader',
    input : ['shaders/test.vert'],
    output : ['vert.spv'],
    command : [
        validator,
        '-V', '@INPUT@',
        '-o', '@OUTPUT@'
    ],
    build_by_default : true
)
fragment = custom_target(
    'fragment_shader',
    input : ['shaders/test.frag'],
    output : ['frag.spv'],
    command : [
        validator,
        '-V', '@INPUT@',
        '-o', '@OUTPUT@'
    ],
    build_by_default : true
)

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
    'src/ImageSampler.cpp',
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
    'VixenEngine',
    engine_sources,
    include_directories : engine_inc,
    dependencies : engine_deps
)
engine_dep = declare_dependency(
    link_with : engine_lib,
    include_directories : engine_inc,
    dependencies : engine_deps
)
