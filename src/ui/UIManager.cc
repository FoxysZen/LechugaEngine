#include <UIManager.h>

UIManager::UIManager()
{

}

UIManager::~UIManager()
{

}

void UIManager::init(UIRenderer *_renderer)
{
    renderer = _renderer;
}

void UIManager::addElement(UIElement *element)
{
    elements.push_back(element);
}

void UIManager::draw()
{
    std::vector<UIElement*> sorted = elements;
    std::sort(sorted.begin(), sorted.end(), [](UIElement* a, UIElement* b) {
        return a->getLayer() < b->getLayer();
    });
    for (UIElement* elem : sorted)
    {
        elem->draw(renderer);
    }
}

bool UIManager::handleClick(int mouseX, int mouseY)
{
    for (UIElement* elem : elements)
    {
        UIButton* button = dynamic_cast<UIButton*>(elem);
        if (button && button->handleClick(mouseX, mouseY))
            return true;
    }
    return false;
}

void UIManager::handleHover(int mouseX, int mouseY)
{
    for (UIElement* elem : elements)
    {
        UIButton* button = dynamic_cast<UIButton*>(elem);
        if (button)
        {
            button->handleHover(mouseX, mouseY);
        }
    }
}

void UIManager::registerCallback(std::string id, 
    std::function<void()> callback)
{
    callbacks[id] = callback;
}
