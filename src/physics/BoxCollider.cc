#include <BoxCollider.h>

BoxCollider::BoxCollider(glm::vec3 _halfExtents)
{
    halfExtents = _halfExtents;
}

ColliderType BoxCollider::getType() const
{
    return ColliderType::BOX;
}

glm::vec3 BoxCollider::getHalfExtents() const
{
    return halfExtents;
}
