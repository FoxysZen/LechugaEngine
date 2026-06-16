#pragma once

#include <Collider.h>

class CapsuleCollider : public Collider
{
    public:
        CapsuleCollider(float _radius, float _height);

        virtual ColliderType getType() const override;
        float getRadius() const;
        float getHeight() const;

    private:
        float radius, height;
};
