#pragma once
#include <Events.h>
#include <EventSystem.h>
#include <glm/glm.hpp>
#include <Logger.h>
#include <memory>
#include <SDL3/SDL.h>
#include <unordered_map>

class InputManager
{
    public:
        InputManager();
        ~InputManager();

        void pollEvents();
        bool isKeyDown(SDL_Keycode key);
        bool wasWindowResized();
        void resetWindowResized();
        void captureMousePosition();
        void updateMouseLast();
        
        glm::vec2 getMouseDelta();
        int getMouseX();
        int getMouseY();
        int getNewWidth();
        int getNewHeight();

    private:
        std::unordered_map<SDL_Keycode, bool> keyState;
        float mouseX, mouseY;
        float lastMouseX, lastMouseY;

        bool windowResized = false;
        int newWidth, newHeight;
};