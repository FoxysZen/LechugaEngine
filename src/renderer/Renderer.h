#pragma once
#include <Camera.h>
#include <glad/glad.h>
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
        void render(const MeshComponent &mesh, const TransformComponent &transform);
        void onResize(int width, int height);
    
    private:
        glm::mat4 currentView;
        glm::mat4 currentProj;
};