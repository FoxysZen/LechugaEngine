#pragma once

#include <ColliderType.h>
#include <glm/glm.hpp>

class Collider
{
    public:
        virtual ~Collider() = default;

        virtual ColliderType getType() const = 0;

        glm::vec3 offset = glm::vec3(0.0f);
};