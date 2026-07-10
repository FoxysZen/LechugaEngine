#pragma once
#include <Camera.h>
#include <Config.h>
#include <Frustum.h>
#include <glad/glad.h>
#include <LightComponent.h>
#include <ParticleComponent.h>
#include <ResourceManager.h>
#include <SDL3/SDL.h>
#include <ShaderProgram.h>
#include <SkinnedMeshComponent.h>
#include <Skydome.h>
#include <TransformComponent.h>

class Renderer
{
    public:
        Renderer(int _width, int _height, ResourceManager *resManager);
        ~Renderer();

        void beginFrame(Camera *camera, float deltaTime);
        void endFrame();
        void render(const SkinnedMeshComponent &sm, 
                    const TransformComponent &transform);
        void setLights(const std::vector<LightComponent> &lights);
        void onResize(int width, int height);
        void setSkydome(Skydome *skydome);
        void drawParticles(const std::vector<ParticleComponent> &particles);
        const std::vector<LightComponent> &getLights();

        int getDrawCalls();
        void addDrawCalls(int n);
        void resetDrawCalls();

        // Shadow Map
        unsigned int getShadowTexture() const;
        glm::mat4 getLightSpaceMatrix() const;
        void beginShadowPass(const glm::vec3 playerPos);
        void endShadowPass();
    
    private:
        void initShadowMapping(ResourceManager *resManager);

        glm::mat4 currentView;
        glm::mat4 currentProj;
        glm::vec3 currentViewPos;
        std::vector<LightComponent> currentLights;
        std::vector<ParticleComponent> currentParticles;

        glm::vec4 BGcolor = glm::vec4(0.04f, 0.32f, 0.30f, 1.0f);

        Frustum frustum;
        Skydome *skydome = nullptr;

        int drawCalls = 0;

        // Shadow Mapping
        unsigned int shadowFBO = 0;
        unsigned int shadowDepthTex = 0;
        const unsigned int SHADOW_WIDTH = 2048;
        const unsigned int SHADOW_HEIGHT = 2048;
        
        glm::mat4 lightSpaceMatrix;
        int screenWidth = 1280;
        int screenHeight = 720;
        bool isShadowPass = true;

        ShaderProgram *shadowShader = nullptr;
};