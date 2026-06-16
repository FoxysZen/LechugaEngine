#pragma once

#include <Collider.h>
#include <vector>

class MeshCollider : public Collider
{
    public:
        MeshCollider(std::vector<glm::vec3> _triangles);

        virtual ColliderType getType() const override;
        const std::vector<glm::vec3> &getTriangles() const;

    private:
        std::vector<glm::vec3> triangles;
};