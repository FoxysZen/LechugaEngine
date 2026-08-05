#pragma once
#include <AudioManager.h>
#include <AnimationSystem.h>
#include <cgltf.h>
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
#include <SkinnedMesh.h>
#include <Texture.h>
#include <TriggerComponent.h>
#include <UIImage.h>
#include <UILabel.h>
#include <UIManager.h>
#include <UIPanel.h>

class SceneLoader
{
    public:
        SceneLoader(Scene *_scene, ResourceManager *_resourceManager, 
                    UIManager *_uiManager, PhysicsEngine *_physicsEngine, 
                    AudioManager *_audioManager);
        ~SceneLoader();

        void loadScene(const std::string &sceneName);
        void saveScene(const std::string &filepath);

    private:
        void createCollider(bool isTrigger, const nlohmann::json &json, int i, 
                            EntityID id, const glm::vec3 &pos, 
                            const glm::vec3 &rot, const glm::vec3 &sca);

        void parseTriggerActions(TriggerComponent &trigger,
                                 const nlohmann::json &jsonArray,
                                 bool isEnter);

        Scene *scene;
        ResourceManager *resourceManager;
        AudioManager *audioManager;
        UIManager *uiManager;
        PhysicsEngine *physicsEngine;
        std::vector<AnimationSystem*> animSystems;
};