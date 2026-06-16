#pragma once
#include <EmitterShape.h>
#include <EntityID.h>
#include <FileSystem.h>
#include <glm/glm.hpp>
#include <ParticleRenderMode.h>
#include <ParticleType.h>
#include <PhysicsEngine.h>
#include <ResourceManager.h>
#include <Scene.h>
#include <ShaderProgram.h>
#include <Texture.h>
#include <UIImage.h>
#include <UILabel.h>
#include <UIManager.h>
#include <UIPanel.h>

class SceneLoader
{
    public:
        SceneLoader(Scene *_scene, ResourceManager *_resourceManager, 
            UIManager *_uiManager, PhysicsEngine *_physicsEngine);
        ~SceneLoader();

        void loadScene(std::string sceneName);

    private:
        Scene *scene;
        ResourceManager *resourceManager;
        UIManager *uiManager;
        PhysicsEngine *physicsEngine;
};