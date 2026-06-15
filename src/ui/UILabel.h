#pragma once

#include <Font.h>
#include <string>
#include <UIElement.h>

class UILabel : public UIElement
{
    public:
        UILabel(int x, int y, std::string text, Font* font);

        void draw(UIRenderer* renderer) override;
        void setText(std::string _text);
        float getScale();
        void setScale(float _scale);
        
    private:
        float fontScale = 1.0f;
        std::string text;
        Font* font;
};