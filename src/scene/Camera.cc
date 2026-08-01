#include <Camera.h>

Camera::Camera(int width, int height, float _fov, float _zN, float _zF)
    : position(0.0f, 0.0f, 0.0f),
      orientation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f))
{
    ra = (float)width / (float)height;
    fov = _fov;
    zN = _zN;
    zF = _zF;
}

Camera::~Camera()
{

}

void Camera::update(InputManager *input, float deltaTime)
{
    if (leftMouse)
    {
        glm::vec2 delta = input->getMouseDelta();
        glm::vec3 currentRight = getRight();

        glm::quat yawQuat   = glm::angleAxis(glm::radians(-delta.x * sensitivity), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::quat pitchQuat = glm::angleAxis(glm::radians(-delta.y * sensitivity), currentRight);

        glm::quat newOrientation = glm::normalize(pitchQuat * yawQuat * orientation);

        glm::vec3 newForward = newOrientation * glm::vec3(0.0f, 0.0f, -1.0f);
        float cosAngle = glm::dot(newForward, glm::vec3(0.0f, 1.0f, 0.0f));

        if (glm::abs(cosAngle) < 0.99f)
        {
            orientation = newOrientation;
        }
        else
        {
            orientation = glm::normalize(yawQuat * orientation);
        }
    }

    glm::vec3 forwardDir = getForward();
    glm::vec3 rightDir   = getRight();

    if (input->isKeyDown(SDLK_W))
    {
        position += forwardDir * speed * deltaTime;
    }
    if (input->isKeyDown(SDLK_S))
    {
        position -= forwardDir * speed * deltaTime;
    }
    if (input->isKeyDown(SDLK_A))
    {
        position -= rightDir * speed * deltaTime;
    }
    if (input->isKeyDown(SDLK_D))
    {
        position += rightDir * speed * deltaTime;
    }

    EventSystem::getInstance().publish(CameraUpdatedEvent{
            getProjectionMatrix() * getViewMatrix()});
}

void Camera::setPosition(const glm::vec3 &newPos)
{
    position = newPos;
}

void Camera::setOrientation(const glm::quat &newRot)
{
    orientation = newRot;
}

void Camera::setFov(const float &_fov)
{
    fov = _fov;
}

void Camera::lookAtPoint(const glm::vec3 &targetPos, const glm::vec3 &up)
{
    glm::vec3 direction = glm::normalize(targetPos - position);
    orientation = glm::quatLookAt(direction, up);
}

void Camera::setTransform(const glm::vec3 &newPos, const glm::quat &newRot)
{
    position = newPos;
    orientation = newRot;
}

glm::mat4 Camera::getViewMatrix() const
{
    glm::mat4 R = glm::mat4_cast(glm::conjugate(orientation));
    glm::mat4 T = glm::translate(glm::mat4(1.0f), -position);
    
    return R * T;
}

glm::mat4 Camera::getProjectionMatrix() const
{
    return glm::perspective(glm::radians(fov), ra, zN, zF);
}

glm::vec3 Camera::getPosition() const
{
    return position;
}

void Camera::setAspectRatio(int width, int height)
{
    ra = (float)width / (float)height;
    EventSystem::getInstance().publish(CameraUpdatedEvent{
            getProjectionMatrix() * getViewMatrix()});
}

void Camera::setLeftMouse(bool state)
{
    leftMouse = state;
}

glm::vec3 Camera::getForward() const
{
    return orientation * glm::vec3(0.0f, 0.0f, -1.0f);
}

glm::vec3 Camera::getRight() const
{
    return orientation * glm::vec3(1.0f, 0.0f, 0.0f);
}
