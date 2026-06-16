#pragma once

#include <BoxCollider.h>
#include <CapsuleCollider.h>
#include <Collider.h>
#include <EntityID.h>
#include <MeshCollider.h>
#include <RigidBody.h>
#include <Scene.h>
#include <SphereCollider.h>
#include <TransformComponent.h>
#include <unordered_map>

struct CollisionInfo
{
    EntityID entity1;
    EntityID entity2;
    glm::vec3 normal;
    float depth;
};

class PhysicsEngine
{
    public:
        PhysicsEngine();
        ~PhysicsEngine();

        void addBody(EntityID id, RigidBody *body);
        void addCollider(EntityID id, Collider *collider);
        void step(float deltaTime, Scene* scene);
        std::vector<CollisionInfo> detectCollisions(Scene* scene);
        void resolveCollision(Scene* scene, CollisionInfo info);

    private:
        std::unordered_map<EntityID, RigidBody*> bodies;
        std::unordered_map<EntityID, Collider*> colliders;
        float gravity;
};