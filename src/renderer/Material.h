#pragma once
#include <glm/glm.hpp>

struct Material
{
    bool cellShaded = true;
    bool foliage = false;
    glm::vec3 diffuse = glm::vec3(1.0f);
    glm::vec3 specular = glm::vec3(1.0f);
    float shin = 0;
};