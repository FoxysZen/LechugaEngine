#pragma once
#include <AnimationSystem.h>
#include <AudioManager.h>
#include <Camera.h>
#include <CharacterController.h>
#include <Config.h>
#include <DebugRenderer.h>
#include <EntityID.h>
#include <InputManager.h>
#include <memory>
#include <ParticleSystem.h>
#include <PhysicsEngine.h>
#include <Renderer.h>
#include <ResourceManager.h>
#include <Scene.h>
#include <SceneLoader.h>
#include <SDL3/SDL.h>
#include <Skydome.h>
#include <Texture.h>
#include <Timer.h>
#include <UIButton.h>
#include <UIImage.h>
#include <UIManager.h>
#include <UILabel.h>
#include <UIPanel.h>
#include <UIRenderer.h>
#include <Window.h> 

class Application
{
    public:
        Application();
        ~Application();

        bool init();
        void run();
        void shutdown();

    private:
        std::unique_ptr<Window> window = nullptr;
        std::unique_ptr<Skydome> skydome = nullptr;
        std::unique_ptr<Scene> scene = nullptr;
        std::unique_ptr<SceneLoader> sceneLoader = nullptr;
        std::unique_ptr<Camera> camera = nullptr;
        std::unique_ptr<Renderer> renderer = nullptr;
        std::unique_ptr<DebugRenderer> debugRenderer = nullptr;
        std::unique_ptr<InputManager> input = nullptr;
        std::unique_ptr<Timer> timer = nullptr;
        std::unique_ptr<ResourceManager> resourceManager = nullptr;

        std::unique_ptr<UIRenderer> uiRenderer = nullptr;
        std::unique_ptr<UIManager> uiManager = nullptr;
        UILabel *debugLabel = nullptr;

        std::unique_ptr<PhysicsEngine> physicsEngine = nullptr;
        std::unique_ptr<CharacterController> characterController = nullptr;
        EntityID playerID;

        std::unique_ptr<AudioManager> audioManager = nullptr;

        std::unique_ptr<AnimationSystem> animSystem = nullptr;

        std::string debugStats = "";

        SkinnedMesh *playerMesh = nullptr;
        ShaderProgram* skinnedShader = nullptr;
};