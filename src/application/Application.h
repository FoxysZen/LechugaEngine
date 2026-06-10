#pragma once
#include <Camera.h>
#include <InputManager.h>
#include <memory>
#include <Renderer.h>
#include <ResourceManager.h>
#include <Scene.h>
#include <SDL3/SDL.h>
#include <Texture.h>
#include <Timer.h>
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
        std::unique_ptr<Scene> scene = nullptr;
        std::unique_ptr<Camera> camera = nullptr;
        std::unique_ptr<Renderer> renderer = nullptr;
        std::unique_ptr<InputManager> input = nullptr;
        std::unique_ptr<Timer> timer = nullptr;
        std::unique_ptr<ResourceManager> resourceManager = nullptr;
};