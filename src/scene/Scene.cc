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

void Scene::addMesh(EntityID id, MeshComponent mesh)
{
    meshes[id] = mesh;
}

void Scene::addLight(EntityID id, LightComponent light)
{
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

TransformComponent *Scene::getTransform(EntityID id)
{
    return &transforms[id];
}

MeshComponent *Scene::getMesh(EntityID id)
{
    return &meshes[id];
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

RigidBody *Scene::getRigidBody(EntityID id)
{
    return rigidBodies[id];
}

void Scene::update(float deltaTime)
{
    for (auto &[id, particle] : particles)
    {
        particle.system->update(deltaTime);
    }
}

void Scene::render(Renderer *renderer)
{
    std::vector<LightComponent> lightList;
    for (auto& [id, light] : lights)
    {
        lightList.push_back(light);
    }
    renderer->setLights(lightList);

    for (auto& [id, mesh] : meshes)
    {
        if (transforms.count(id) > 0)
        {
            renderer->render(mesh, transforms[id]);
        }
    }

    glDepthMask(GL_FALSE);
    std::vector<ParticleComponent> particleList;
    for (auto& [id, particle] : particles)
    {
        particleList.push_back(particle);
    }
    renderer->drawParticles(particleList);
    glDepthMask(GL_TRUE);
}
