project(
    'VixenEngine',
    'cpp',
    version : '0.0.1',
    default_options : ['cpp_std=c++20']
)

if get_option('vixen_debug')
    add_project_arguments('-DVIXEN_DEBUG', language : 'cpp')
endif

subdir('engine')
subdir('editor')
