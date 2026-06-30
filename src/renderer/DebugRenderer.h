#pragma once
#include <BoxCollider.h>
#include <CapsuleCollider.h>
#include <Collider.h>
#include <ColliderType.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <SphereCollider.h>
#include <TransformComponent.h>
#include <vector>

class DebugRenderer
{
public:
    DebugRenderer();
    ~DebugRenderer();

    void init();
    void draw(Collider *collider, TransformComponent *transform, 
              glm::mat4 viewProj, glm::vec3 color = {0.0f, 1.0f, 0.0f});
    void cleanup();
    bool isVisible();
    void setVisible(bool v);

private:
    GLuint VAO, VBO, EBO;
    GLuint shaderProgram;

    void drawSphere(SphereCollider *col, TransformComponent *t, glm::mat4 viewProj, glm::vec3 color);
    void drawBox(BoxCollider *col, TransformComponent *t, glm::mat4 viewProj, glm::vec3 color);
    void drawCapsule(CapsuleCollider *col, TransformComponent *t, glm::mat4 viewProj, glm::vec3 color);

    void uploadAndDraw(const std::vector<glm::vec3> &verts,
                       const std::vector<unsigned int> &indices,
                       glm::mat4 mvp, glm::vec3 color);

    GLuint compileShader(const char *vert, const char *frag);

    std::vector<glm::vec3> buildSphereWire(float radius, int slices = 12);
    std::vector<glm::vec3> buildBoxWire(glm::vec3 halfExtents);
    std::vector<glm::vec3> buildCapsuleWire(float radius, float height, 
                                            int stacks = 8, int slices = 12);

    bool visible = false;
};