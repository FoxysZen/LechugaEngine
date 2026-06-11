#pragma once
#include <EntityID.h>
#include <LightComponent.h>
#include <MeshComponent.h>
#include <Renderer.h>
#include <TransformComponent.h>
#include <unordered_map>

class Scene
{
    public:
        Scene();
        ~Scene();

        EntityID createEntity();
        void addTransform(EntityID id, TransformComponent transform);
        void addMesh(EntityID id, MeshComponent mesh);
        void addLight(EntityID id, LightComponent light);
        TransformComponent getTransform(EntityID id);
        MeshComponent getMesh(EntityID id);
        LightComponent getLight(EntityID id);
        void update(float deltaTime);
        void render(Renderer *renderer);

    
    private:
        uint32_t nextID = 0;
        std::unordered_map<EntityID, TransformComponent> transforms;
        std::unordered_map<EntityID, MeshComponent> meshes;
        std::unordered_map<EntityID, LightComponent> lights;
};