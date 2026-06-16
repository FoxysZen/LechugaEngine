#include <CapsuleCollider.h>

CapsuleCollider::CapsuleCollider(float _radius, float _height)
{
    radius = _radius;
    height = _height;
}

ColliderType CapsuleCollider::getType() const
{
    return ColliderType::CAPSULE;
}

float CapsuleCollider::getRadius() const
{
    return radius;
}

float CapsuleCollider::getHeight() const
{
    return height;
}
