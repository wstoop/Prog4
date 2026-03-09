#pragma once
#include "Component.h"

namespace dae
{
    class TransformComponent;

    class BulletComponent final : public Component
    {
    public:
        BulletComponent(GameObject* owner);
        void Activate(float x, float y, float dirX, float dirY, float speed);
        void Deactivate();
        void Update() override;

    private:
        float m_dirX{};
        float m_dirY{};
        float m_speed{};
        bool m_active{ false };
        TransformComponent* m_transform{};
    };
}