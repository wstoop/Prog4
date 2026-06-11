#pragma once
#include "Components/Component.h"

namespace dae
{
    class TransformComponent;
	class TextureComponent;
    class ShootComponent final : public Component
    {
    public:
        ShootComponent(GameObject* owner, float speed);
        void Shoot(float dirX, float dirY);
        void Update() override;
    private:
        float m_speed;
        TransformComponent* m_transform;
        TextureComponent* m_sprite;

		float m_ShootCooldown{ 0.f };
		float m_ShootDelay{ 0.5f };
    };
}