#pragma once
#include <glm/glm.hpp>

struct LightComponent {
    glm::vec3 position;
    glm::vec3 color;
    float intensity;
};