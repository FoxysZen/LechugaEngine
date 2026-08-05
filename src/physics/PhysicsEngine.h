#pragma once

#include <BoxCollider.h>
#include <CapsuleCollider.h>
#include <Collider.h>
#include <EntityID.h>
#include <Grid.h>
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

        void addPlayer(EntityID id);
        void addBody(EntityID id, RigidBody *body);
        void addCollider(EntityID id, Collider *collider);
        void addTriggerCollider(EntityID id, Collider *collider);

        void step(float deltaTime, Scene *scene);
        void detectCollisions(Scene *scene, std::vector<CollisionInfo> &info);
        void detectTriggerCollisions(Scene *scene);
        void resolveCollision(Scene *scene, CollisionInfo info);
        void getCollisionsFor(EntityID id, Scene *scene, 
                              std::vector<CollisionInfo> &outCollisions);
        bool testCollision(
                    Collider *col1, TransformComponent *trans1, EntityID id1,
                    Collider *col2, TransformComponent *trans2, EntityID id2,
                    CollisionInfo &result);
        void testMeshCollisions(Collider *col, TransformComponent *trans, 
                                EntityID id, std::vector<CollisionInfo> &info);

    private:
        glm::vec3 closestPointOnTriangle(glm::vec3 P,
                                         glm::vec3 A,
                                         glm::vec3 B,
                                         glm::vec3 C);

        glm::vec3 closestPointOnSegment(glm::vec3 A,
                                        glm::vec3 B,
                                        glm::vec3 P,
                                        float invLengthSq);

        void closestPointsBetweenSegments(glm::vec3 p1, glm::vec3 q1, 
                                          glm::vec3 p2, glm::vec3 q2,
                                          glm::vec3 &c1, glm::vec3 &c2);

        std::vector<RigidBody*> bodies;
        std::unordered_map<EntityID, Collider*> colliders;
        std::unordered_map<EntityID, Collider*> triggerColliders;
        float gravity = 2.0f;

        std::vector<EntityID> dynamicObjects; // RigidBody and !isKinematic
        std::vector<EntityID> staticObjects;  // !RigidBody or MESH

        std::vector<CollisionInfo> collisions;

        // AABB
        std::unordered_map<uint64_t, GridCell> cells;
        float cellSize = 5.0f;
};