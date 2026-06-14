#include <UIButton.h>

UIButton::UIButton(int _x, int _y, int _width, int _height, 
                   Texture* _normal, Texture* _hover, 
                   std::function<void()> _onClick)
{
    x = _x;
    y = _y;
    width = _width;
    height = _height;
    normalTexture = _normal;
    hoverTexture = _hover;
    onClick = _onClick;
}

UIButton::~UIButton() {}

void UIButton::draw(UIRenderer* renderer)
{
    if (!visible) return;
    renderer->drawTexturedQuad(x, y, width, height, 
        hovered ? hoverTexture : normalTexture);
}

bool UIButton::handleClick(int mouseX, int mouseY)
{
    if (mouseX >= x && mouseX <= x + width &&
        mouseY >= y && mouseY <= y + height)
    {
        onClick();
        return true;
    }
    return false;
}

void UIButton::handleHover(int mouseX, int mouseY)
{
    hovered = (mouseX >= x && mouseX <= x + width &&
               mouseY >= y && mouseY <= y + height);
}