#include <InputManager.h>

InputManager::InputManager() {}

InputManager::~InputManager() {}

void InputManager::pollEvents()
{   
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_EVENT_QUIT:
                EventSystem::getInstance().publish(QuitEvent{});
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                windowResized = true;
                newWidth = event.window.data1;
                newHeight = event.window.data2;
                EventSystem::getInstance().publish(
                        WindowResizedEvent{newWidth, newHeight});
                break;
            case SDL_EVENT_MOUSE_MOTION:
                mouseX = event.motion.x;
                mouseY = event.motion.y;
                break;
            case SDL_EVENT_KEY_DOWN:
                keyState[event.key.key] = true;
                EventSystem::getInstance().publish(
                        KeyPressedEvent{event.key.key});
                break;
            case SDL_EVENT_KEY_UP:
                keyState[event.key.key] = false;
                EventSystem::getInstance().publish(
                        KeyReleasedEvent{event.key.key});
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    EventSystem::getInstance().publish(
                        LeftMousePressedEvent{});
                }
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    EventSystem::getInstance().publish(
                        LeftMouseReleasedEvent{});
                }
                break;
            default:
            //Logger::info("Window: Unhanheld event.");
            break;
        }
    }
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
    return glm::vec2(mouseX - lastMouseX, mouseY - lastMouseY);
}

void InputManager::captureMousePosition()
{
    float x, y;
    SDL_GetMouseState(&x, &y);
    lastMouseX = x;
    lastMouseY = y;
}

int InputManager::getMouseX()
{
    return (int)mouseX;
}

int InputManager::getMouseY()
{
    return (int)mouseY;
}


int InputManager::getNewWidth()
{
    return newWidth;
}

int InputManager::getNewHeight()
{
    return newHeight;
}

void InputManager::updateMouseLast()
{
    lastMouseX = mouseX;
    lastMouseY = mouseY;
}
