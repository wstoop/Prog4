#pragma once
#include "Component.h"
#include <glm/glm.hpp>

namespace dae
{
    class OrbitComponent final : public Component
    {
        float m_radius{};
        float m_speed{};
        float m_angle{};
    public:
        OrbitComponent(GameObject* owner, float radius, float speed, float startAngle = 0.f);

        void Update() override;

        void SetRadius(float radius) { m_radius = radius; }
        void SetSpeed(float speed) { m_speed = speed; }
    };
}