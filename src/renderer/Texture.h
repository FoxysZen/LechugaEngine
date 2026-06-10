#pragma once
#include <glad/glad.h>
#include <Logger.h>
#include <stb_image.h>
#include <string>

class Texture
{
    public:
        Texture();
        ~Texture();

        void load(std::string path);
        void bind(int slot);
        void unbind(int slot);

    private:
        GLuint textureID;
};