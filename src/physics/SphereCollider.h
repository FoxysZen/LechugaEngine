#pragma once

#include <Collider.h>

class SphereCollider : public Collider
{
    public:
        SphereCollider(float _radius, glm::vec3 _offset);
    
        virtual ColliderType getType() const override;
        float getRadius() const;

    private:
        float radius;
};