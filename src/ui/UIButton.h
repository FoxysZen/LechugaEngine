#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <UIElement.h>

class UIButton : public UIElement
{
    public:
        UIButton(int x, int y, int width, int height, Texture* normal, 
            Texture* hover, std::function<void()> onClick);
        ~UIButton();

        void draw(UIRenderer* renderer) override;
        bool handleClick(int mouseX, int mouseY);
        void handleHover(int mouseX, int mouseY);

    private:
        Texture* normalTexture;
        Texture* hoverTexture;
        bool hovered = false;
        std::function<void()> onClick;
};