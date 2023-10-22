project('nightmare', 'c', version : '0.1')

subdir('src')

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : false)
x11_dep = dependency('x11')
egl_dep = dependency('egl')
gles1_dep = dependency('glesv1_cm')
gles2_dep = dependency('glesv2')

common_dependencies = [m_dep, egl_dep, x11_dep]
include_directories = include_directories([
    'src',
    'src/scenes'
])

executable('nightmare-gles1',
            src_sources,
           dependencies : common_dependencies + gles1_dep,
           install: true,
           c_args : '-DNIGHTMARE_USE_GLES1',
           include_directories: include_directories)

executable('nightmare-gles2',
           src_sources,
           dependencies : common_dependencies + gles2_dep,
           install: true,
           c_args : '-DNIGHTMARE_USE_GLES2',
           include_directories: include_directories)
