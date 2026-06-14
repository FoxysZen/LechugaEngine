#include <Texture.h>

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

