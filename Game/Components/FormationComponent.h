#pragma once
#include "Components/Component.h"
#include <glm/glm.hpp>

namespace dae
{
    class TransformComponent;
    class FormationComponent final : public Component
    {
        float m_time{ 0.f };
        float m_horizontalOffset{ 30.f };
        bool m_moveLeft{ true };
        bool m_started{ false };
        TransformComponent* m_transform{ nullptr };
        int m_allEnemies{ 999 };
        int m_dockedCount{ 0 };

        float m_breathTime{};
        float m_breathSpeed{ 0.8f };
        float m_breathAmount{ 3.f };

        void LeftRight();
        void AdvanceBreath();

    public:
        FormationComponent(GameObject* owner);

        void Update() override;
        void SetAllEnemies(int enemyCount);

        // Called by EnemyFormationSlotComponent when an enemy docks
        void NotifyDocked();
        void NotifyDied();
        // Pure query — no enemy pointers needed
        glm::vec3 ComputeSwayOffset(const glm::vec3& slotLocalPos, const glm::vec3& center) const;
    };
}