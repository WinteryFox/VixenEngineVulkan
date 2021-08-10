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

editor = executable(
    'Vixen Editor',
    'main.cpp',
    dependencies : [
        engine_dep
    ]
)
