#pragma once
#include "Component.h"

namespace dae
{
    class TransformComponent;
	class TextureComponent;
    class ShootComponent final : public Component
    {
    public:
        ShootComponent(GameObject* owner, float speed);
        void Shoot(float dirX, float dirY);

    private:
        float m_speed;
        TransformComponent* m_transform;
        TextureComponent* m_sprite;
    };
}