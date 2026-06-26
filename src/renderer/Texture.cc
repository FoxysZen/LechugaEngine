#include <Texture.h>
#include <cstring>

Texture::Texture()
{

}

Texture::~Texture()
{
    glDeleteTextures(1, &textureID);
}

void Texture::load(std::string path)
{
    int _width, _height, channels;
    unsigned char* data = stbi_load(
        path.c_str(), 
        &_width, 
        &_height, 
        &channels, 
        0
    );
    if (!data)
    {
        Logger::error("Texture: could not load " + path + '.');
        return;
    }

    width = _width;
    height = _height;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Pixelated Textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 
        0, 
        format, 
        width, 
        height, 
        0, 
        format, 
        GL_UNSIGNED_BYTE, 
        data
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

void Texture::loadFromMemory(unsigned char* pixels, int width, int height)
{
    int rowSize = width * 4;
    std::vector<unsigned char> flipped(rowSize * height);
    for (int y = 0; y < height; ++y)
    {
        memcpy(flipped.data() + y * rowSize, 
               pixels + (height - 1 - y) * rowSize, rowSize);
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, flipped.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind(int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::unbind(int slot)
{
    glBindTexture(GL_TEXTURE_2D, slot);
}

int Texture::getWidth()
{
    return width;
}

int Texture::getHeight()
{
    return height;
}

