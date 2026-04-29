#pragma once
#include "Components/Component.h"

namespace dae
{
    class TransformComponent;

    class BulletComponent final : public Component
    {
    public:
        BulletComponent(GameObject* owner);
        void Activate(GameObject* shooter, float x, float y, float dirX, float dirY, float speed);
        void Deactivate();
		bool IsActive() const { return m_active; }
		void SetActive(bool active) { m_active = active; }
        GameObject* GetShooter() const { return m_shooter; }
        void Update() override;

    private:
        float m_dirX{};
        float m_dirY{};
        float m_speed{};
        bool m_active{ false };
        TransformComponent* m_transform{};
		GameObject* m_shooter{};
    };
}