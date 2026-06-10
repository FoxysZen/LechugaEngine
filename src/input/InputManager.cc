#include <InputManager.h>

InputManager::InputManager() {}

InputManager::~InputManager() {}

bool InputManager::pollEvents()
{   
    SDL_Event event;
    bool state = true;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_EVENT_QUIT:
                state = false;
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                windowResized = true;
                newWidth = event.window.data1;
                newHeight = event.window.data2;
                break;
            case SDL_EVENT_MOUSE_MOTION:
                mouseX = event.motion.xrel;
                mouseY = event.motion.yrel;
                break;
            case SDL_EVENT_KEY_DOWN:
                keyState[event.key.key] = true;
                break;
            case SDL_EVENT_KEY_UP:
                keyState[event.key.key] = false;
                break;
            default:
            //Logger::info("Window: Unhanheld event.");
            break;
        }
    }
    SDL_GetRelativeMouseState(&mouseX, &mouseY);
    
    return state;
}

bool InputManager::isKeyDown(SDL_Keycode key)
{
    return keyState[key];
}

bool InputManager::wasWindowResized()
{
    return windowResized;
}

void InputManager::resetWindowResized()
{
    windowResized = false;
}

glm::vec2 InputManager::getMouseDelta()
{
    return glm::vec2(mouseX, mouseY);
}

int InputManager::getNewWidth()
{
    return newWidth;
}

int InputManager::getNewHeight()
{
    return newHeight;
}
