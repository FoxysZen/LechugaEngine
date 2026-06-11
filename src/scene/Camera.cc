#include <Camera.h>

Camera::Camera(int width, int height)
{
    ra = (float)width / (float)height;
    fov = 45.0f;
    zN = 0.1f;
    zF = 1000.0f;
}

Camera::~Camera()
{

}

void Camera::update(InputManager *input, float deltaTime)
{
    glm::vec2 delta = input->getMouseDelta();
    yaw += delta.x * sensitivity;
    pitch -= delta.y * sensitivity;
    pitch = glm::clamp(pitch, -89.0f, 89.0f);
    updateVectors();

    glm::vec3 right = glm::normalize(glm::cross(forward, upVec));

    if (input->isKeyDown(SDLK_W))
    {
        position += forward * speed * deltaTime;
    }
    if (input->isKeyDown(SDLK_S))
    {
        position -= forward * speed * deltaTime;
    }
    if (input->isKeyDown(SDLK_A))
    {
        position -= right * speed * deltaTime;
    }
    if (input->isKeyDown(SDLK_D))
    {
        position += right * speed * deltaTime;
    }
}

void Camera::updateVectors()
{
    forward.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    forward.y = glm::sin(glm::radians(pitch));
    forward.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    forward = glm::normalize(forward);
}

glm::mat4 Camera::getViewMatrix()
{
    glm::mat4 view = glm::lookAt(position, position + forward, upVec);
    return view;
}

glm::mat4 Camera::getProjectionMatrix()
{
    glm::mat4 proj = glm::perspective(glm::radians(fov), ra, zN, zF);
    return proj;
}

glm::vec3 Camera::getPosition()
{
    return position;
}

void Camera::setAspectRatio(int width, int height)
{
    ra = (float)width / (float)height;
}
