#version 450

struct Particle {
   vec3 position;
   vec3 velocity;
};


layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(set = 0, binding = 1) readonly buffer particles {
   Particle part[];
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 texCoord;

void main() {
    vec3 pos = part[gl_InstanceIndex].position;
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition + pos, 1.0);
    texCoord = inTexCoord;
}
