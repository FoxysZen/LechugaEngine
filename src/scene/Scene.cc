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

TransformComponent Scene::getTransform(EntityID id)
{
    return transforms[id];
}

MeshComponent Scene::getMesh(EntityID id)
{
    return meshes[id];
}

LightComponent Scene::getLight(EntityID id)
{
    return lights[id];
}

void Scene::update(float deltaTime)
{
    // TODO: update fzx
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
}
