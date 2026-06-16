#include <MeshCollider.h>

MeshCollider::MeshCollider(std::vector<glm::vec3> _triangles)
{
    triangles = _triangles;
}

ColliderType MeshCollider::getType() const
{
    return ColliderType::MESH;
}

const std::vector<glm::vec3> &MeshCollider::getTriangles() const
{
    return triangles;
}
