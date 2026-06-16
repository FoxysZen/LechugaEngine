#pragma once

#include <Collider.h>

class BoxCollider : public Collider
{
    public:
        BoxCollider(glm::vec3 _halfExtents);

        virtual ColliderType getType() const override;
        glm::vec3 getHalfExtents() const;
    
    private:
        glm::vec3 halfExtents;
};