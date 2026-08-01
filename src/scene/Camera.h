#pragma once
#include "glm/ext/vector_float3.hpp"
#include <Config.h>
#include <Events.h>
#include <EventSystem.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <InputManager.h>
#include <SDL3/SDL.h>

class Camera
{
    public:
        Camera(int width, int height, float fov, float zN, float zF);
        ~Camera();

        void update(InputManager *input, float deltaTime);

        void setAspectRatio(int width, int height);
        void setLeftMouse(bool state);
        void setPosition(const glm::vec3 &newPos);
        void setOrientation(const glm::quat &newRot);
        void setFov(const float &_fov);
        
        void setTransform(const glm::vec3 &newPos, const glm::quat &newRot);
        void lookAtPoint(const glm::vec3 &targetPos, 
                         const glm::vec3 &up = glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 getViewMatrix() const;
        glm::mat4 getProjectionMatrix() const;
        glm::vec3 getPosition() const;
        glm::vec3 getForward() const;
        glm::vec3 getRight() const;
    
    private:
        glm::vec3 position = glm::vec3(0.0f, 5.0f, 5.0f);
        glm::quat orientation;
        glm::vec3 upVec = glm::vec3(0.0f, 1.0f, 0.0f);
        float fov, ra, zN, zF;

        float speed = 10.0f;
        float sensitivity = 0.1f;
        glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);

        bool leftMouse = false;
};