#include <UIImage.h>

UIImage::UIImage(int _x, int _y, int _width, int _height, Texture *_texture)
{
    x = _x;
    y = _y;
    width = _width;
    height = _height;
    texture = _texture;
    layer = 0;
}

UIImage::~UIImage() {}

void UIImage::draw(UIRenderer *renderer)
{
    if (!visible) return;
    renderer->drawTexturedQuad(x, y, width, height, texture);
}
