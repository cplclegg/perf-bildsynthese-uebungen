#version 450

layout(location = 0) out vec3 fragColor;

vec2 positions[6] = vec2[](
    vec2(-0.4, 0.4), // tl   first triangle green
    vec2(-0.4, -0.4),// bl
    vec2(0.4, -0.4), // br

    vec2(-0.4, 0.4), // tl   second triangle green
    vec2(0.4, -0.4), // br
    vec2(0.4, 0.4)   // tr green

);

vec3 colors[6] = vec3[](
    vec3(1.0, 0.0, 0.0), // first triangle
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0),
    vec3(1.0, 0.0, 0.0),  // second triangle
    vec3(0.0, 0.0, 1.0),
    vec3(0.0, 1.0, 0.0)
);

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragColor = colors[gl_VertexIndex];
}
