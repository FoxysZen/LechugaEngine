#pragma once
#include <EntityID.h>
#include <glm/glm.hpp>

struct GridCell
{
    std::vector<std::pair<EntityID, size_t>> trisIndex;
};