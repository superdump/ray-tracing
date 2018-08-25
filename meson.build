project('Ray Tracing in One Week', 'cpp',
    default_options : [
        'cpp_std=c++14',
        'warning_level=1',
        'buildtype=debugoptimized'
    ])
rtiow_includes = include_directories('subprojects/stb')
rtiow_sources = [
    'src/aabb.h',
    'src/aarect.h',
    'src/box.h',
    'src/bvh.h',
    'src/camera.h',
    'src/constant_medium.h',
    'src/hitable.h',
    'src/hitable_list.h',
    'src/main.cpp',
    'src/material.h',
    'src/moving_sphere.h',
    'src/perlin.h',
    'src/ray.h',
    'src/rng.h',
    'src/sphere.h',
    'src/surface_texture.h',
    'src/texture.h',
    'src/vec3.h'
]
rtiow_deps = [
    dependency('enkiTS', fallback: ['enkiTS', 'enkiTS_dep']),
    dependency('minifb', fallback: ['minifb', 'minifb_dep']),
    dependency('threads')
]
executable('rtiow', rtiow_sources,
    dependencies: rtiow_deps,
    include_directories: rtiow_includes,
    install: true)
install_data('data/world.topo.bathy.200412.3x5400x2700.jpg',
    install_dir : join_paths(get_option('datadir'), 'rtiow'))
