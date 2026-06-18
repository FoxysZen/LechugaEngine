#pragma once

#include <EntityID.h>
#include <glm/glm.hpp>
#include <PhysicsEngine.h>
#include <Scene.h>

class CharacterController
{
    public:
        CharacterController(EntityID _id, PhysicsEngine *_physics, 
                            Scene *_scene);
        ~CharacterController();

        void move(glm::vec3 direction, float speed, float deltaTime);
        bool isGrounded();
        void setGrounded(bool g);

    private:
        EntityID id;

        PhysicsEngine *physics;
        Scene *scene;

        glm::vec3 velocity;

        float gravity = 2.0f;
        bool grounded = false;
};