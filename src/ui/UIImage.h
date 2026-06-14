#pragma once

#include <glm/glm.hpp>
#include <Texture.h>
#include <UIElement.h>

class UIImage : public UIElement
{
    public:
        UIImage(int _x, int _y, int _width, int _height, Texture *_texture);
        ~UIImage();
        
        void draw(UIRenderer* renderer) override;

    private:
        Texture *texture;
};