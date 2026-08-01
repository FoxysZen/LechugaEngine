#include "EntityID.h"
#include <Scene.h>

Scene::Scene()
{

}

Scene::~Scene()
{

}

EntityID Scene::createEntity()
{
    return nextID++;
}

void Scene::addTransform(EntityID id, TransformComponent transform)
{
    transforms[id] = transform;
}

void Scene::addSkinnedMesh(EntityID id, SkinnedMeshComponent component)
{
    skinnedMeshes[id] = component;
}

void Scene::addLight(EntityID id, LightComponent light)
{
    if (isSun)
    {
        sunPos = light.position;
        isSun = false;
    }
    lights[id] = light;
}

void Scene::addParticle(EntityID id, ParticleComponent particle)
{
    particles[id] = particle;
}

void Scene::addCollider(EntityID id, ColliderComponent collider)
{
    colliders[id] = collider;
}

void Scene::addRigidBody(EntityID id, RigidBody *rigidbody)
{
    rigidBodies[id] = rigidbody;
}

void Scene::addMaterial(EntityID id, Material material)
{
    materials[id] = material;
}

TransformComponent *Scene::getTransform(EntityID id)
{
    return &transforms[id];
}

SkinnedMeshComponent *Scene::getSkinnedMesh(EntityID id)
{
    return &skinnedMeshes[id];
}

LightComponent *Scene::getLight(EntityID id)
{
    return &lights[id];
}

ParticleComponent *Scene::getParticle(EntityID id)
{
    return &particles[id];
}

ColliderComponent *Scene::getCollider(EntityID id)
{
    return &colliders[id];
}

std::unordered_map<EntityID, ColliderComponent> *Scene::getColliderMap()
{
    return &colliders;
}

std::unordered_map<EntityID, SkinnedMeshComponent> *Scene::getSkinnedMeshMap()
{
    return &skinnedMeshes;
}

std::unordered_map<EntityID, TransformComponent> *Scene::getTransformMap()
{
    return &transforms;
}
std::unordered_map<EntityID, LightComponent> *Scene::getLights()
{
    return &lights;
}

RigidBody *Scene::getRigidBody(EntityID id)
{
    return rigidBodies[id];
}

Material *Scene::getMaterial(EntityID id)
{
    return &materials[id];
}

void Scene::update(float deltaTime)
{
    // Update Sun
    sunSeconds += deltaTime;

    float speedMultiplier = 0.05f;
    float currentAngle = sunSeconds * speedMultiplier;

    sunPos = glm::vec3(
        std::sin(currentAngle) * 10.0f,
        sunPos.y,
        std::cos(currentAngle) * 10.0f
    );

    for (auto &[id, particle] : particles)
        particle.system->update(deltaTime);

    for (auto &[id, sm] : skinnedMeshes)
        if (sm.animSys) sm.animSys->update(deltaTime);
}

void Scene::render(Renderer *renderer)
{
    std::vector<LightComponent> lightList;
    for (auto& [id, light] : lights)
    {
        lightList.push_back(light);
    }
    renderer->setLights(lightList);

    // Normal Render
    for (auto &[id, sm] : skinnedMeshes)
    {
        if (transforms.count(id))
        {
            if (materials.count(id))
            {
                renderer->render(sm, transforms[id], true, materials[id]);
            }
            else
            {
                static const Material dummyMat{};
                renderer->render(sm, transforms[id], false, dummyMat);
            }
        }
    }

    glDepthMask(GL_FALSE);
    std::vector<ParticleComponent> particleList;
    for (auto &[id, particle] : particles)
    {
        particleList.push_back(particle);
    }
    renderer->drawParticles(particleList);
    glDepthMask(GL_TRUE);
}
