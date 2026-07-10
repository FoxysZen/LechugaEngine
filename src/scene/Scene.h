#pragma once
#include <ColliderComponent.h>
#include <EntityID.h>
#include <LightComponent.h>
#include <ParticleComponent.h>
#include <Renderer.h>
#include <RigidBody.h>
#include <SkinnedMeshComponent.h>
#include <TransformComponent.h>
#include <unordered_map>

class Scene
{
    public:
        Scene();
        ~Scene();

        EntityID createEntity();

        void addTransform(EntityID id, TransformComponent transform);
        void addSkinnedMesh(EntityID id, SkinnedMeshComponent component);
        void addLight(EntityID id, LightComponent light);
        void addParticle(EntityID id, ParticleComponent particle);
        void addCollider(EntityID id, ColliderComponent collider);
        void addRigidBody(EntityID id, RigidBody *rigidbody);
        void drawParticles(const std::unordered_map<EntityID, 
                            ParticleComponent>& particles);

        TransformComponent *getTransform(EntityID id);
        SkinnedMeshComponent *getSkinnedMesh(EntityID id);
        LightComponent *getLight(EntityID id);
        ParticleComponent *getParticle(EntityID id);
        ColliderComponent *getCollider(EntityID id);
        std::unordered_map<EntityID, ColliderComponent> *getColliderMap();
        RigidBody *getRigidBody(EntityID id);
        std::unordered_map<EntityID, SkinnedMeshComponent> *getSkinnedMeshMap();

        void update(float deltaTime);
        void render(Renderer *renderer, EntityID playerId);
    
    private:
        uint32_t nextID = 0;
        std::unordered_map<EntityID, TransformComponent> transforms;
        std::unordered_map<EntityID, SkinnedMeshComponent> skinnedMeshes;
        std::unordered_map<EntityID, LightComponent> lights;
        std::unordered_map<EntityID, ParticleComponent> particles;
        std::unordered_map<EntityID, ColliderComponent> colliders;
        std::unordered_map<EntityID, RigidBody*> rigidBodies;
};