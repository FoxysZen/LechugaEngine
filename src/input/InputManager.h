#pragma once
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

        bool pollEvents();
        bool isKeyDown(SDL_Keycode key);
        bool wasWindowResized();
        void resetWindowResized();
        
        glm::vec2 getMouseDelta();
        int getNewWidth();
        int getNewHeight();

    private:
        std::unordered_map<SDL_Keycode, bool> keyState;
        float mouseX, mouseY;

        bool windowResized = false;
        int newWidth, newHeight;
};