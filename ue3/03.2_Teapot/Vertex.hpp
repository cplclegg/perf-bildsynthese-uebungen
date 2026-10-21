#pragma once

#include <glm/glm.hpp>

typedef float Vector2[2];
typedef float Vector3[3];

struct Vertex {
    Vector3 pos;
    Vector2 tex;
    Vector3 nor;
};

typedef Vertex Triangle[3];