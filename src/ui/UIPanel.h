#pragma once

#include <glm/glm.hpp>
#include <UIElement.h>

class UIPanel : public UIElement
{
    public:
        UIPanel(int _x, int _y, int _width, int _height, glm::vec3 _color);
        ~UIPanel();
        
        void draw(UIRenderer *renderer) override;

    private:
        glm::vec3 color;
};