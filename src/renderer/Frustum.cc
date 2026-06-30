#include <Frustum.h>

Frustum::Frustum()
{

}

Frustum::~Frustum()
{
    
}

void Frustum::update(glm::mat4 projView)
{
    glm::mat4 m = glm::transpose(projView);

    planes[0] = m[3] + m[0]; // left
    planes[1] = m[3] - m[0]; // right
    planes[2] = m[3] + m[1]; // bottom
    planes[3] = m[3] - m[1]; // top
    planes[4] = m[3] + m[2]; // near
    planes[5] = m[3] - m[2]; // far

    for (auto& plane : planes)
    {
        plane /= glm::length(glm::vec3(plane));
    }
}

bool Frustum::isSphereInside(glm::vec3 center, float radius)
{
    for (auto &plane : planes)
    {
        float dist = glm::dot(glm::vec3(plane), center) + plane.w;
        if (dist < -radius)
            return false;
    }
    return true;
}
