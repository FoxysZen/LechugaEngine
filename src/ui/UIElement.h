#pragma once

#include <UIRenderer.h>

class UIElement
{
    public:
        virtual ~UIElement() = default;

        void setVisible(bool visible) { this->visible = visible; }
        bool isVisible() { return visible; }
        virtual void draw(UIRenderer *renderer) = 0;
        int getLayer() { return layer; }
        void setLayer(int n) { layer = n; }

    protected:
        int x, y, width, height;
        int layer = 0;
        bool visible = true;
};