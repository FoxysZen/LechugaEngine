#pragma once

#include <UIRenderer.h>

class UIElement
{
    public:
        virtual ~UIElement() = default;

        void setVisible(bool visible) { this->visible = visible; }
        virtual void draw(UIRenderer* renderer) = 0;

    protected:
        int x, y, width, height;
        bool visible = true;
};