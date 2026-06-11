#pragma once
#include <Camera.h>
#include <Config.h>
#include <Frustum.h>
#include <glad/glad.h>
#include <LightComponent.h>
#include <memory>
#include <MeshComponent.h>
#include <ResourceManager.h>
#include <SDL3/SDL.h>
#include <ShaderProgram.h>
#include <TransformComponent.h>

class Renderer
{
    public:
        Renderer();
        ~Renderer();

        void beginFrame(Camera *camera);
        void render(const MeshComponent &mesh, 
                    const TransformComponent &transform);
        void setLights(const std::vector<LightComponent>& lights);
        void onResize(int width, int height);
    
    private:
        glm::mat4 currentView;
        glm::mat4 currentProj;
        glm::vec3 currentViewPos;
        std::vector<LightComponent> currentLights;

        glm::vec4 BGcolor = glm::vec4(0.04f, 0.32f, 0.30f, 1.0f);

        Frustum frustum;
};