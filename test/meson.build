test = executable(
    'engine_test',
    'main.cpp',
    dependencies : [
        engine_dep
    ]
)

validator = find_program('glslangValidator')
run_command(
    validator,
    '-V', 'shaders/test.vert',
    '-o', meson.current_build_dir() + '/vert.spv'
)
run_command(
    validator,
    '-V', 'shaders/test.frag',
    '-o', meson.current_build_dir() + '/frag.spv'
)

test(
    'engine test',
    test,
    workdir : meson.current_build_dir()
)
