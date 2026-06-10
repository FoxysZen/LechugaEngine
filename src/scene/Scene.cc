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

TransformComponent Scene::getTransform(EntityID id)
{
    return transforms[id];
}

MeshComponent Scene::getMesh(EntityID id)
{
    return meshes[id];
}

void Scene::update(float deltaTime)
{
    // TODO: update fzx
}

void Scene::render(Renderer *renderer)
{
    for (auto& [id, mesh] : meshes)
    {
        if (transforms.count(id) > 0)
        {
            renderer->render(mesh, transforms[id]);
        }
    }
}
