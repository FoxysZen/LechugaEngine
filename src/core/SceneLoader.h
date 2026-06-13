#pragma once
#include <EmitterShape.h>
#include <EntityID.h>
#include <FileSystem.h>
#include <glm/glm.hpp>
#include <ParticleRenderMode.h>
#include <ParticleType.h>
#include <ResourceManager.h>
#include <Scene.h>
#include <ShaderProgram.h>
#include <Texture.h>

class SceneLoader
{
    public:
        SceneLoader(Scene *scene, ResourceManager *resourceManager);
        ~SceneLoader();

        void loadScene(std::string sceneName);

    private:
        Scene *scene;
        ResourceManager *resourceManager;
};