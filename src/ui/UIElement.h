#pragma once

#include <UIRenderer.h>

class UIElement
{
    public:
        virtual ~UIElement() = default;

        void setVisible(bool visible) { this->visible = visible; }
        virtual void draw(UIRenderer* renderer) = 0;
        int getLayer() { return layer; }

    protected:
        int x, y, width, height;
        int layer = 0;
        bool visible = true;
};