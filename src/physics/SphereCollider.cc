#include <SphereCollider.h>

SphereCollider::SphereCollider(float _radius, glm::vec3 _offset)
{
    radius = _radius;
    offset = _offset;
}
    
ColliderType SphereCollider::getType() const
{
    return ColliderType::SPHERE;
}

float SphereCollider::getRadius() const
{
    return radius;
}
