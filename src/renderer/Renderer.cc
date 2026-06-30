#include <Renderer.h>

Renderer::Renderer()
{
    EventSystem::getInstance().subscribe<CameraUpdatedEvent>
        ([this](const CameraUpdatedEvent &e) {
            frustum.update(e.projView);
    });
}

Renderer::~Renderer()
{
    
}

void Renderer::beginFrame(Camera *camera, float deltaTime)
{
    currentView = camera->getViewMatrix();
    currentProj = camera->getProjectionMatrix();
    currentViewPos = camera->getPosition();
    
    BGcolor = Config::getInstance().clearColor;

    glClearColor(BGcolor.r, BGcolor.g, BGcolor.b, BGcolor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    if (skydome)
    {
        skydome->draw(currentView, currentProj, deltaTime);
        ++drawCalls;
    }

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

void Renderer::render(const SkinnedMeshComponent &sm, 
    const TransformComponent &transform)
{
    glm::mat4 model = transform.getModelMatrix();
    glm::vec3 worldCenter = glm::vec3(model * glm::vec4(
        sm.lods[0].mesh->getBoundsCenter(), 1.0f));
    float dist = glm::length(currentViewPos - worldCenter);
    
    SkinnedMesh *activeMesh = sm.lods.back().mesh;
    for (auto &lod : sm.lods)
    {
        if (dist < lod.maxDistance)
        {
            activeMesh = lod.mesh;
            break;
        }
    }

    glm::vec3 scale = glm::vec3(
        glm::length(glm::vec3(model[0])),
        glm::length(glm::vec3(model[1])),
        glm::length(glm::vec3(model[2]))
    );
    float worldRadius = activeMesh->getBoundsRadius() * glm::max(scale.x, 
                                                glm::max(scale.y, scale.z));
    if (!frustum.isSphereInside(worldCenter, worldRadius))
    {
        return;
    }

    sm.shader->bind();
    sm.shader->setUniformMat4("view", currentView);
    sm.shader->setUniformMat4("proj", currentProj);
    sm.shader->setUniformMat4("model", model);
    sm.shader->setUniformInt("textures[0]", 0);

    int size = currentLights.size();
    sm.shader->setUniformInt("numLights", size);
    for (int i = 0; i < size; ++i)
    {
        glm::vec3 lightPosCameraSpace = glm::vec3(currentView * glm::vec4(
            currentLights[i].position, 1.0f));
        sm.shader->setUniformVec3("lightPos[" + std::to_string(i) + "]", 
            lightPosCameraSpace);
        sm.shader->setUniformVec3("lightColor[" + std::to_string(i) + "]", 
            currentLights[i].color);
    }

    if (sm.animSys)
    {
        const auto &matrices = sm.animSys->getBoneMatrices();
        int numBones = (int)matrices.size();
        for (int i = 0; i < numBones; ++i)
        {
            sm.shader->setUniformMat4("boneMatrices[" + std::to_string(i) + "]", 
                matrices[i]);
        }
    }

    activeMesh->draw();
    ++drawCalls;

    sm.shader->unbind();
}

void Renderer::setLights(const std::vector<LightComponent> &lights)
{
    currentLights = lights;
}

void Renderer::setSkydome(Skydome *skydome)
{
    this->skydome = skydome;
}

void Renderer::onResize(int width, int height)
{
    glViewport(0, 0, width, height);
}

void Renderer::drawParticles(const std::vector<ParticleComponent> &particles)
{
    for (auto &particle : particles)
    {
        glm::vec3 center = particle.system->getBoundsCenter();
        float radius = particle.system->getBoundsRadius();
        if (frustum.isSphereInside(center, radius))
        {
            particle.system->draw(currentView, currentProj);
            ++drawCalls;
        }
    }
}

const std::vector<LightComponent> &Renderer::getLights()
{
    return currentLights;
}

int Renderer::getDrawCalls()
{
    return drawCalls;
}

void Renderer::addDrawCalls(int n)
{
    drawCalls += n;
}

void Renderer::resetDrawCalls()
{
    drawCalls = 0;
}
