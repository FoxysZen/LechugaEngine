#include <Renderer.h>

Renderer::Renderer()
{

}

Renderer::~Renderer()
{
    
}

void Renderer::beginFrame(Camera *camera)
{
    currentView = camera->getViewMatrix();
    currentProj = camera->getProjectionMatrix();
    glClearColor(0.04f, 0.32f, 0.30f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
}

void Renderer::render(const MeshComponent &mesh, const TransformComponent &transform)
{
    mesh.shader->bind();
    mesh.shader->setUniformMat4("view", currentView);
    mesh.shader->setUniformMat4("proj", currentProj);
    mesh.shader->setUniformMat4("model", transform.getModelMatrix());
    mesh.shader->setUniformInt("texture0", 0);
    mesh.shader->setUniformInt("texture1", 1);
    int size = mesh.textures.size();
    for (int i = 0; i < size; ++i)
    {
        mesh.textures[i]->bind(i);
    }
    mesh.mesh->draw();
    for (int i = 0; i < size; ++i)
    {
        mesh.textures[i]->unbind(i);
    }
    mesh.shader->unbind();
}

void Renderer::onResize(int width, int height)
{
    glViewport(0, 0, width, height);
}
