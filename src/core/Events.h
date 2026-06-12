#pragma once
#include <SDL3/SDL.h>
#include <glm/glm.hpp>

struct WindowResizedEvent { int width, height; };
struct KeyPressedEvent { SDL_Keycode key; };
struct KeyReleasedEvent { SDL_Keycode key; };
struct LeftMousePressedEvent {};
struct LeftMouseReleasedEvent {};
struct QuitEvent {};
struct CameraUpdatedEvent { glm::mat4 projView; };