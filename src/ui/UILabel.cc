#include <UILabel.h>

UILabel::UILabel(int _x, int _y, std::string _text, Font* _font)
{
    x = _x;
    y = _y;
    text = _text;
    font = _font;

    layer = 2;
}

void UILabel::draw(UIRenderer* renderer)
{
    if (!visible) return;
    renderer->drawText(x, y, text, font);
}