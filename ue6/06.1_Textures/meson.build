project(
    'Performante Bildsynthese mit Vulkan WS2627',
    'cpp',
    default_options : [
        'cpp_std=c++23',
        'warning_level=3'
        ]
)

deps = [
    dependency('glfw3'),
    dependency('xrandr'),
    dependency('x11'),
    dependency('xxf86vm'),
    dependency('vulkan'),
    dependency('xi'),
]

headers = [
    'Device.hpp',
    'HelloVulkan10.hpp',
    'Instance.hpp',
    'stb_image.h',
    'Texture.hpp',
    'Vertex.hpp',
    'VertexBuffer.hpp',
    'VulkanUtils.hpp',
]

sources = [
    'Device.cpp',
    'Instance.cpp',
    'Texture.cpp',
    'VertexBuffer.cpp',
]

glslang = find_program('glslangValidator')

shader_sources = [
    'testapp.vert',
    'testapp.frag',
##    'testapp.comp',
]

foreach shader : shader_sources
    custom_target(
        shader + '.spv',
        input: 'shaders/' + shader,
        output: shader + '.spv',
        command: [glslang, '-V', '-o', '@OUTPUT@', '@INPUT@'],
        build_by_default: true
    )
endforeach

testapp = executable(
    'testapp',
    'main.cpp',
    sources,
    headers,
    dependencies: deps
)
