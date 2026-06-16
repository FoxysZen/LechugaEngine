#pragma once

#include <glm/glm.hpp>

class RigidBody
{
    public:
        RigidBody();
        ~RigidBody();

        void init(glm::vec3 _velocity, glm::vec3 _accel, float _mass, 
                  bool _useGravity, bool _isKinematic);
        void applyForce(glm::vec3 force);
        void applyImpulse(glm::vec3 impulse);

        glm::vec3 getVelocity() const;
        glm::vec3 getAcceleration() const;
        float getMass() const;
        bool getUseGravity() const;
        bool getIsKinematic() const;
        void setVelocity(glm::vec3 v);
        void setAcceleration(glm::vec3 a);

    private:
        glm::vec3 velocity;
        glm::vec3 acceleration;

        float mass;

        bool useGravity;
        bool isKinematic;
};