#pragma once

#include <UIRenderer.h>

class UIElement
{
    public:
        virtual ~UIElement() = default;

        virtual void draw(UIRenderer *renderer) = 0;

        int getLayer() { return layer; }
        glm::vec4 getParameters() { return glm::vec4(x, y, width, height); }
        
        void setVisible(bool visible) { this->visible = visible; }
        void setLayer(int n) { layer = n; }

        bool isVisible() { return visible; }

    protected:
        int x, y, width, height;
        int layer = 0;
        bool visible = true;
};