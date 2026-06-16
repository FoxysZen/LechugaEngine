#include <RigidBody.h>

RigidBody::RigidBody() {}

RigidBody::~RigidBody() {}

void RigidBody::init(glm::vec3 _velocity, glm::vec3 _accel, float _mass, 
          bool _useGravity, bool _isKinematic)
{
    velocity = _velocity;
    acceleration = _accel;
    mass = _mass;
    useGravity = _useGravity;
    isKinematic = _isKinematic;
}

void RigidBody::applyForce(glm::vec3 force)
{
    acceleration += force / mass;
}

void RigidBody::applyImpulse(glm::vec3 impulse)
{
    velocity += impulse;
}

glm::vec3 RigidBody::getVelocity() const
{
    return velocity;
}

glm::vec3 RigidBody::getAcceleration() const
{
    return acceleration;
}

float RigidBody::getMass() const
{
    return mass;
}

bool RigidBody::getUseGravity() const
{
    return useGravity;
}

bool RigidBody::getIsKinematic() const
{
    return isKinematic;
}

void RigidBody::setVelocity(glm::vec3 v)
{
    velocity = v;
}

void RigidBody::setAcceleration(glm::vec3 a)
{
    acceleration = a;
}
