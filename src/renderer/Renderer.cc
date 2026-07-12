#include "ResourceManager.h"
#include <Renderer.h>

Renderer::Renderer(int _width, int _height, ResourceManager *resManager)
{
    EventSystem::getInstance().subscribe<CameraUpdatedEvent>
        ([this](const CameraUpdatedEvent &e) {
            frustum.update(e.projView);
    });

    screenWidth = _width;
    screenHeight = _height;

    initShadowMapping(resManager);
}

Renderer::~Renderer()
{
    if (shadowFBO != 0)
        glDeleteFramebuffers(1, &shadowFBO);
    if (shadowDepthTex != 0)
        glDeleteTextures(1, &shadowDepthTex);
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

void Renderer::endFrame()
{
    glUseProgram(0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
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

    // Shadow Map
    if (isShadowPass)
    {
        if (!shadowShader) return;

        shadowShader->bind();
        shadowShader->setUniformMat4("lightSpaceMatrix", lightSpaceMatrix);
        shadowShader->setUniformMat4("model", model);

        // Animation Shadow
        if (sm.animSys)
        {
            const auto &matrices = sm.animSys->getBoneMatrices();
            int numBones = (int)matrices.size();
            for (int i = 0; i < numBones; ++i)
            {
                shadowShader->setUniformMat4("boneMatrices[" + std::to_string(i) + "]", 
                                             matrices[i]);
            }
        }

        activeMesh->draw();

        shadowShader->unbind();
        return;
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

    sm.shader->setUniformMat4("lightSpaceMatrix", lightSpaceMatrix);

    sm.shader->setUniformInt("textures[0]", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, shadowDepthTex);
    sm.shader->setUniformInt("shadowMap", 1); 
    glActiveTexture(GL_TEXTURE0);

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
    screenWidth = width;
    screenHeight = height;
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

unsigned int Renderer::getShadowTexture() const
{
    return shadowDepthTex;
}

glm::mat4 Renderer::getLightSpaceMatrix() const
{
    return lightSpaceMatrix;
}

void Renderer::initShadowMapping(ResourceManager *resManager)
{
    glGenFramebuffers(1, &shadowFBO);

    glGenTextures(1, &shadowDepthTex);
    glBindTexture(GL_TEXTURE_2D, shadowDepthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, 
                 SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    // Pixelated Shadow
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 
                           shadowDepthTex, 0);
    
    // No colors
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    shadowShader = resManager->loadShader("assets/shaders/shadow.vert",
                                          "assets/shaders/shadow.frag");
}

void Renderer::beginShadowPass(const glm::vec3 sunPos, const glm::vec3 playerPos)
{
    if (currentLights.empty()) return;

    isShadowPass = true;

    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glClear(GL_DEPTH_BUFFER_BIT);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.0f, 4.0f);

    float lightDistance = 40.0f; 
    glm::vec3 normalizedSunPos = glm::normalize(sunPos);
    glm::vec3 lightTarget = playerPos;
    glm::vec3 lightPos = lightTarget + (normalizedSunPos * lightDistance);

    float boxSize = 20.0f;
    float near_plane = 1.0f;
    float far_plane  = 120.0f;
    
    glm::mat4 lightProjection = glm::ortho(-boxSize, boxSize, -boxSize, 
                                           boxSize, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(lightPos, lightTarget, 
                                      glm::vec3(0.0f, 1.0f, 0.0f));

    lightSpaceMatrix = lightProjection * lightView;

    shadowShader->bind();
    shadowShader->setUniformMat4("lightSpaceMatrix", lightSpaceMatrix);
}

void Renderer::endShadowPass()
{
    glDisable(GL_POLYGON_OFFSET_FILL);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, screenWidth, screenHeight);

    isShadowPass = false;
}
