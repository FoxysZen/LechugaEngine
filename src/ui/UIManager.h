#pragma once

#include <algorithm>
#include <unordered_map>
#include <UIButton.h>
#include <UIElement.h>
#include <UIRenderer.h>
#include <vector>

class UIManager
{
    public:
        UIManager();
        ~UIManager();

        void init(UIRenderer* _renderer);
        void addElement(UIElement* element);
        void draw();
        bool handleClick(int mouseX, int mouseY);
        void handleHover(int mouseX, int mouseY);

        void registerCallback(std::string id, std::function<void()> callback);
        std::unordered_map<std::string, std::function<void()>> callbacks;

    private:
        std::vector<UIElement*> elements;
        UIRenderer* renderer;
};