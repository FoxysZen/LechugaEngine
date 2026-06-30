#include "DebugRenderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

static const char *VERT_SRC = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
uniform mat4 uMVP;
void main() { gl_Position = uMVP * vec4(aPos, 1.0); }
)";

static const char *FRAG_SRC = R"(
#version 330 core
uniform vec3 uColor;
out vec4 FragColor;
void main() { FragColor = vec4(uColor, 1.0); }
)";

DebugRenderer::DebugRenderer() : VAO(0), VBO(0), EBO(0), shaderProgram(0) {}
DebugRenderer::~DebugRenderer() { cleanup(); }

void DebugRenderer::init()
{
    shaderProgram = compileShader(VERT_SRC, FRAG_SRC);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
}

void DebugRenderer::cleanup()
{
    if (VAO) { glDeleteVertexArrays(1, &VAO); VAO = 0; }
    if (VBO) { glDeleteBuffers(1, &VBO);      VBO = 0; }
    if (EBO) { glDeleteBuffers(1, &EBO);      EBO = 0; }
    if (shaderProgram) { glDeleteProgram(shaderProgram); shaderProgram = 0; }
}

bool DebugRenderer::isVisible()
{
    return visible;
}

void DebugRenderer::setVisible(bool v)
{
    visible = v;
}

void DebugRenderer::draw(Collider *collider, TransformComponent *transform,
                         glm::mat4 viewProj, glm::vec3 color)
{
    switch (collider->getType())
    {
        case ColliderType::SPHERE:
            drawSphere(dynamic_cast<SphereCollider*>(collider), transform, viewProj, color);
            break;
        case ColliderType::BOX:
            drawBox(dynamic_cast<BoxCollider*>(collider), transform, viewProj, color);
            break;
        case ColliderType::CAPSULE:
            drawCapsule(dynamic_cast<CapsuleCollider*>(collider), transform, viewProj, color);
            break;
        default: break;
    }
}

std::vector<glm::vec3> DebugRenderer::buildSphereWire(float radius, int slices)
{
    std::vector<glm::vec3> verts;
    auto addCircle = [&](auto xFn, auto yFn, auto zFn)
    {
        for (int i = 0; i <= slices; ++i)
        {
            float a = glm::two_pi<float>() * i / slices;
            verts.push_back({ xFn(a) * radius, yFn(a) * radius, zFn(a) * radius });
        }
    };
    addCircle([](float a){ return cosf(a); }, [](float a){ return sinf(a); }, [](float){ return 0.0f; }); // XY
    addCircle([](float a){ return cosf(a); }, [](float){ return 0.0f; }, [](float a){ return sinf(a); }); // XZ
    addCircle([](float){ return 0.0f; }, [](float a){ return cosf(a); }, [](float a){ return sinf(a); }); // YZ
    return verts;
}

std::vector<glm::vec3> DebugRenderer::buildBoxWire(glm::vec3 h)
{
    return {
        {-h.x,-h.y,-h.z}, { h.x,-h.y,-h.z}, { h.x, h.y,-h.z}, {-h.x, h.y,-h.z}, // back
        {-h.x,-h.y, h.z}, { h.x,-h.y, h.z}, { h.x, h.y, h.z}, {-h.x, h.y, h.z}  // front
    };
}

std::vector<glm::vec3> DebugRenderer::buildCapsuleWire(float radius, float height, int stacks, int slices)
{
    std::vector<glm::vec3> verts;
    float halfH = height / 2.0f;

    for (int i = 0; i < slices; ++i)
    {
        float a = glm::two_pi<float>() * i / slices;
        float x = cosf(a) * radius;
        float z = sinf(a) * radius;
        verts.push_back({x, -halfH, z});
        verts.push_back({x,  halfH, z});
    }

    for (int cap = 0; cap < 2; ++cap)
    {
        float y = (cap == 0) ? -halfH : halfH;
        for (int i = 0; i <= slices; ++i)
        {
            float a = glm::two_pi<float>() * i / slices;
            verts.push_back({cosf(a) * radius, y, sinf(a) * radius});
        }
    }

    for (int cap = 0; cap < 2; ++cap)
    {
        float yBase = (cap == 0) ? -halfH : halfH;
        float sign  = (cap == 0) ? -1.0f  : 1.0f;
        for (int s = 0; s <= stacks; ++s)
        {
            float phi = glm::half_pi<float>() * s / stacks;
            float y   = yBase + sign * sinf(phi) * radius;
            float r   = cosf(phi) * radius;
            for (int i = 0; i <= slices; ++i)
            {
                float a = glm::two_pi<float>() * i / slices;
                verts.push_back({cosf(a) * r, y, sinf(a) * r});
            }
        }
    }
    return verts;
}

void DebugRenderer::drawSphere(SphereCollider *col, TransformComponent *t,
                                glm::mat4 viewProj, glm::vec3 color)
{
    int slices = 24;
    auto verts = buildSphereWire(col->getRadius(), slices);

    std::vector<unsigned int> indices;
    for (int c = 0; c < 3; ++c)
    {
        int base = c * (slices + 1);
        for (int i = 0; i < slices; ++i)
        {
            indices.push_back(base + i);
            indices.push_back(base + i + 1);
        }
    }

    glm::mat4 model = glm::translate(glm::mat4(1.0f), t->position + col->offset);
    uploadAndDraw(verts, indices, viewProj * model, color);
}

void DebugRenderer::drawBox(BoxCollider *col, TransformComponent *t,
                             glm::mat4 viewProj, glm::vec3 color)
{
    auto verts = buildBoxWire(col->getHalfExtents());

    std::vector<unsigned int> indices = {
        0,1, 1,2, 2,3, 3,0,
        4,5, 5,6, 6,7, 7,4,
        0,4, 1,5, 2,6, 3,7
    };

    glm::mat4 model = glm::translate(glm::mat4(1.0f), t->position + col->offset);
    uploadAndDraw(verts, indices, viewProj * model, color);
}

void DebugRenderer::drawCapsule(CapsuleCollider *col, TransformComponent *t,
                                 glm::mat4 viewProj, glm::vec3 color)
{
    int slices = 16;
    auto verts = buildCapsuleWire(col->getRadius(), col->getHeight(), 8, slices);

    std::vector<unsigned int> indices;
    int offset = 0;

    for (int i = 0; i < slices; ++i)
    {
        indices.push_back(offset + i * 2);
        indices.push_back(offset + i * 2 + 1);
    }
    offset += slices * 2;

    for (int cap = 0; cap < 2; ++cap)
    {
        int base = offset + cap * (slices + 1);
        for (int i = 0; i < slices; ++i)
        {
            indices.push_back(base + i);
            indices.push_back(base + i + 1);
        }
    }
    offset += 2 * (slices + 1);

    int stacks = 8;
    for (int cap = 0; cap < 2; ++cap)
    {
        int capBase = offset + cap * (stacks + 1) * (slices + 1);
        for (int s = 0; s <= stacks; ++s)
        {
            int ringBase = capBase + s * (slices + 1);
            for (int i = 0; i < slices; ++i)
            {
                indices.push_back(ringBase + i);
                indices.push_back(ringBase + i + 1);
            }
        }
    }

    glm::mat4 model = glm::translate(glm::mat4(1.0f), t->position + col->offset);
    uploadAndDraw(verts, indices, viewProj * model, color);
}

void DebugRenderer::uploadAndDraw(const std::vector<glm::vec3> &verts,
                                   const std::vector<unsigned int> &indices,
                                   glm::mat4 mvp, glm::vec3 color)
{
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uMVP"), 1, 
                                            GL_FALSE, glm::value_ptr(mvp));
    glUniform3fv(glGetUniformLocation(shaderProgram, "uColor"), 1, 
                                      glm::value_ptr(color));

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(glm::vec3), 
                 verts.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                 indices.size() * sizeof(unsigned int), indices.data(), 
                 GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    GLboolean depthWasEnabled = glIsEnabled(GL_DEPTH_TEST);
    glDisable(GL_DEPTH_TEST);

    glDrawElements(GL_LINES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);

    if (depthWasEnabled) glEnable(GL_DEPTH_TEST);

    glBindVertexArray(0);
}

GLuint DebugRenderer::compileShader(const char *vert, const char *frag)
{
    auto compile = [](GLenum type, const char *src) -> GLuint
    {
        GLuint s = glCreateShader(type);
        glShaderSource(s, 1, &src, nullptr);
        glCompileShader(s);
        return s;
    };

    GLuint v = compile(GL_VERTEX_SHADER,   vert);
    GLuint f = compile(GL_FRAGMENT_SHADER, frag);
    GLuint p = glCreateProgram();
    glAttachShader(p, v);
    glAttachShader(p, f);
    glLinkProgram(p);
    glDeleteShader(v);
    glDeleteShader(f);
    return p;
}