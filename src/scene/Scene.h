#pragma once
#include <ColliderComponent.h>
#include <EntityID.h>
#include <LightComponent.h>
#include <Material.h>
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
        void addMaterial(EntityID id, Material material);

        TransformComponent *getTransform(EntityID id);
        SkinnedMeshComponent *getSkinnedMesh(EntityID id);
        LightComponent *getLight(EntityID id);
        ParticleComponent *getParticle(EntityID id);
        ColliderComponent *getCollider(EntityID id);
        RigidBody *getRigidBody(EntityID id);
        Material *getMaterial(EntityID id);

        std::unordered_map<EntityID, ColliderComponent> *getColliderMap();
        std::unordered_map<EntityID, SkinnedMeshComponent> *getSkinnedMeshMap();
        std::unordered_map<EntityID, TransformComponent> *getTransformMap();
        std::unordered_map<EntityID, LightComponent> *getLights();

        void update(float deltaTime);
        void render(Renderer *renderer);
        void updateShadowCamera(float deltaTime, glm::vec3 playerPos);
        void drawParticles(const std::unordered_map<EntityID, 
                           ParticleComponent>& particles);
    
    private:
        uint32_t nextID = 0;
        std::unordered_map<EntityID, TransformComponent> transforms;
        std::unordered_map<EntityID, SkinnedMeshComponent> skinnedMeshes;
        std::unordered_map<EntityID, LightComponent> lights;
        std::unordered_map<EntityID, ParticleComponent> particles;
        std::unordered_map<EntityID, ColliderComponent> colliders;
        std::unordered_map<EntityID, RigidBody*> rigidBodies;
        std::unordered_map<EntityID, Material> materials;

        // Sun movement
        bool isSun = true;
        float sunSeconds = 0.0f;
        glm::vec3 sunPos = glm::vec3(0.0f);
};