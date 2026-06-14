#pragma once

#include <Font.h>
#include <string>
#include <UIElement.h>

class UILabel : public UIElement
{
    public:
        UILabel(int x, int y, std::string text, Font* font);

        void draw(UIRenderer* renderer) override;
        
    private:
        std::string text;
        Font* font;
};