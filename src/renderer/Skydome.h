#pragma once
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <ShaderProgram.h>
#include <Texture.h>
#include <vector>

class Skydome
{
    public:
        Skydome();
        ~Skydome();

        void init(ShaderProgram *shader, Texture *texture);
        void draw(const glm::mat4 &view, const glm::mat4 &proj, float deltaTime);
        std::vector<float> generateSphere(float radius, int rings, int sectors);

    private:
        const float PI = glm::pi<float>();

        GLuint VAO, VBO;
        ShaderProgram *shader;
        Texture *texture;
        float time = 0;
        float scrollSpeed = 0.01f;
        int vertexCount;
};