#pragma once
#include <glad/glad.h>
#include <Logger.h>
#include <stb_image.h>
#include <string>
#include <vector>

class Texture
{
    public:
        Texture();
        ~Texture();

        void load(std::string path);
        void loadFromMemory(unsigned char* pixels, int width, int height);
        void bind(int slot);
        void unbind(int slot);

        int getWidth();
        int getHeight();

    private:
        GLuint textureID;
        int width, height;
};