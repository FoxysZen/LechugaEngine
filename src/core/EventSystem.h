#pragma once
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <vector>

class EventSystem
{
    public:
        static EventSystem &getInstance()
        {
            static EventSystem instance;
            return instance;
        }
        // delete copy/move
        EventSystem(const EventSystem&) = delete;
        EventSystem& operator=(const EventSystem&) = delete;

        template<typename T>
        void subscribe(std::function<void(const T&)> callback)
        {
            subscribers[std::type_index(typeid(T))].push_back(
                [callback](const void *event) {
                    callback(*static_cast<const T*>(event));
                }
            );
        }
    
        template<typename T>
        void publish(const T &event)
        {
            auto it = subscribers.find(std::type_index(typeid(T)));
            if (it != subscribers.end())
                for (auto &cb : it->second)
                    cb(&event);
        }
    
    private:
        EventSystem() {}
        std::unordered_map<std::type_index, std::vector<std::function<void(const void*)>>> subscribers;
};