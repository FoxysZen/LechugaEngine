#pragma once
#include <EntityID.h>
#include <functional>
#include <unordered_set>

using TriggerCallback = std::function<void(EntityID entityID)>;

struct TriggerComponent {
    std::vector<TriggerCallback> onEnterCallbacks;
    std::vector<TriggerCallback> onExitCallbacks;

    std::unordered_set<EntityID> entitiesInside;

    void onEnter(TriggerCallback callback)
    {
        onEnterCallbacks.push_back(callback);
    }

    void onExit(TriggerCallback callback)
    {
        onExitCallbacks.push_back(callback);
    }

    void notifyEnter(EntityID id)
    {
        for (auto &cb : onEnterCallbacks)
            cb(id);
    }

    void notifyExit(EntityID id)
    {
        for (auto &cb : onExitCallbacks)
            cb(id);
    }
};
