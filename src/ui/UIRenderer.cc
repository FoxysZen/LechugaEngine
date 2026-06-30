#include <UIRenderer.h>

UIRenderer::UIRenderer() {}
UIRenderer::~UIRenderer() {}

void UIRenderer::init(ShaderProgram *_shader, int _width, int _height)
{
    shader = _shader;
    width = _width;
    height = _height;
    projection = glm::ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);

    // quad + UV (x,y,u,v)
    float quad[] = {
        0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_DYNAMIC_DRAW);
    // Position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    // UV
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 
        (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void UIRenderer::drawQuad(int x, int y, int width, int height, glm::vec3 color)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    float quad[] = {
        0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f
    };
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad), quad);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(width, height, 1.0f));
    shader->bind();
    shader->setUniformMat4("model", model);
    shader->setUniformMat4("projection", projection);
    shader->setUniformVec3("color", color);
    shader->setUniformInt("useTexture", 0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    shader->unbind();

    glDisable(GL_BLEND);
}

void UIRenderer::drawTexturedQuad(int x, int y, int width, int height, 
    Texture *texture)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    float quad[] = {
        0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f
    };
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad), quad);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(width, height, 1.0f));
    shader->bind();
    shader->setUniformMat4("model", model);
    shader->setUniformMat4("projection", projection);
    shader->setUniformInt("useTexture", 1);
    shader->setUniformInt("uiTexture", 0);
    texture->bind(0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    texture->unbind(0);
    shader->unbind();

    glDisable(GL_BLEND);
}

void UIRenderer::drawTexturedQuad(int x, int y, int width, int height, 
                                   Texture *texture, int srcX, int srcY, 
                                   int srcW, int srcH)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    float u0 = (float)srcX / texture->getWidth();
    float v0 = (float)srcY / texture->getHeight();
    float u1 = (float)(srcX + srcW) / texture->getWidth();
    float v1 = (float)(srcY + srcH) / texture->getHeight();

    float quad[] = {
        0.0f, 0.0f, u0, v0,
        1.0f, 0.0f, u1, v0,
        1.0f, 1.0f, u1, v1,
        0.0f, 0.0f, u0, v0,
        1.0f, 1.0f, u1, v1,
        0.0f, 1.0f, u0, v1,
    };

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad), quad);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(width, height, 1.0f));
    shader->bind();
    shader->setUniformMat4("model", model);
    shader->setUniformMat4("projection", projection);
    shader->setUniformInt("useTexture", 1);
    shader->setUniformInt("uiTexture", 0);
    texture->bind(0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    texture->unbind(0);
    shader->unbind();

    glDisable(GL_BLEND);
}

void UIRenderer::drawText(int x, int y, std::string text, Font *font, 
                          float scale)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    int cursorX = x;
    int cursorY = y;

    for (unsigned char c : text)
    {
        if (c == '\n')
        {
            cursorX = x;
            cursorY += (int)(font->lineHeight * scale);
            continue;
        }
        int id = (int)c;
        if (font->chars.find(id) == font->chars.end()) continue;
        CharInfo &info = font->chars[id];
        if (info.width > 0 && info.height > 0)
        {
            drawTexturedQuad(
                cursorX + (int)(info.xoffset * scale),
                cursorY + (int)(info.yoffset * scale),
                (int)(info.width * scale),
                (int)(info.height * scale),
                font->texture,
                info.x, info.y,
                info.width, info.height
            );
            cursorX += (int)(info.width * scale);
        }
        else
        {
            cursorX += (int)(info.xadvance * scale);
        }
    }
    glDisable(GL_BLEND);
}
