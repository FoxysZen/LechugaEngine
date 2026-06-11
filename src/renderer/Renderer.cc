#include <Renderer.h>

Renderer::Renderer()
{
    EventSystem::getInstance().subscribe<CameraUpdatedEvent>
        ([this](const CameraUpdatedEvent& e) {
            frustum.update(e.projView);
    });
}

Renderer::~Renderer()
{
    
}

void Renderer::beginFrame(Camera *camera)
{
    currentView = camera->getViewMatrix();
    currentProj = camera->getProjectionMatrix();
    currentViewPos = camera->getPosition();
    
    BGcolor = Config::getInstance().clearColor;

    glClearColor(BGcolor.r, BGcolor.g, BGcolor.b, BGcolor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

void Renderer::render(const MeshComponent &mesh, const TransformComponent &transform)
{
    glm::mat4 model = transform.getModelMatrix();
    glm::vec3 worldCenter = glm::vec3(model * glm::vec4(
            mesh.mesh->getBoundsCenter(), 1.0f));

    glm::vec3 scale = glm::vec3(
        glm::length(glm::vec3(model[0])),
        glm::length(glm::vec3(model[1])),
        glm::length(glm::vec3(model[2]))
    );

    float worldRadius = mesh.mesh->getBoundsRadius() * glm::max(
            scale.x, glm::max(scale.y, scale.z));

    if (!frustum.isSphereInside(worldCenter, worldRadius))
    {
        return;
    }

    mesh.shader->bind();
    mesh.shader->setUniformMat4("view", currentView);
    mesh.shader->setUniformMat4("proj", currentProj);
    mesh.shader->setUniformMat4("model", transform.getModelMatrix());
    for (int i = 0; i < mesh.textures.size(); i++)
    {
        mesh.shader->setUniformInt("textures[" + std::to_string(i) + "]", i);
    }

    int size = mesh.textures.size();
    for (int i = 0; i < size; ++i)
    {
        mesh.textures[i]->bind(i);
    }

    if (!currentLights.empty())
    {
        glm::vec3 lightPosCameraSpace = 
           glm::vec3(currentView * glm::vec4(currentLights[0].position, 1.0f));
        mesh.shader->setUniformVec3("lightPos", lightPosCameraSpace);
        mesh.shader->setUniformVec3("lightColor", currentLights[0].color);
    }

    mesh.mesh->draw();

    for (int i = 0; i < size; ++i)
    {
        mesh.textures[i]->unbind(i);
    }
    mesh.shader->unbind();
}

void Renderer::setLights(const std::vector<LightComponent>& lights)
{
    currentLights = lights;
}

void Renderer::onResize(int width, int height)
{
    glViewport(0, 0, width, height);
}
