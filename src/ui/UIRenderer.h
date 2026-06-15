#pragma once

#include <CharInfo.h>
#include <Font.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Logger.h>
#include <ShaderProgram.h>
#include <Texture.h>

class UIRenderer
{
    public:
        UIRenderer();
        ~UIRenderer();

        void init(ShaderProgram *_shader, int _width, int _height);
        void drawQuad(int x, int y, int _width, int _height, glm::vec3 color);
        void drawTexturedQuad(int x, int y, int _width, int _height, 
            Texture *texture);
        void drawTexturedQuad(int x, int y, int width, int height, 
            Texture* texture, int srcX, int srcY, int srcWidth, int srcHeight);
        void drawText(int x, int y, std::string text, Font* font, float scale);

    private:
        GLuint VAO, VBO;
        ShaderProgram* shader;
        glm::mat4 projection;
        int width, height;
};