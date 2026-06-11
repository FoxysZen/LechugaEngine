#pragma once
#include <glm/glm.hpp>

class Frustum
{
    public:
        Frustum();
        ~Frustum();

        void update(glm::mat4 projView);
        bool isSphereInside(glm::vec3 center, float radius);

    private:
        glm::vec4 planes[6];
};