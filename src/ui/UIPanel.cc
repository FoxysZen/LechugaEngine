#include <UIPanel.h>

UIPanel::UIPanel(int _x, int _y, int _width, int _height, glm::vec3 _color)
{
    x = _x;
    y = _y;
    width = _width;
    height = _height;
    color = _color;
}

UIPanel::~UIPanel() {}

void UIPanel::draw(UIRenderer* renderer)
{
    if (!visible) return;
    renderer->drawQuad(x, y, width, height, color);
}
