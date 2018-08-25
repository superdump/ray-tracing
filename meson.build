project('Ray Tracing in One Week', 'cpp', default_options : ['cpp_std=c++14'])
deps = [
    dependency('enkiTS', fallback: ['enkiTS', 'enkiTS_dep']),
    dependency('minifb', fallback: ['minifb', 'minifb_dep']),
    dependency('threads')
]
executable('rtiow', 'main.cpp', dependencies: deps)
