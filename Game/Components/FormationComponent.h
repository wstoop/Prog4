#pragma once
#include "Components/Component.h"
#include "EventManager.h"
#include <glm/glm.hpp>

namespace dae
{
    class TransformComponent;
	class FormationComponent final : public Component, public EventHandler
    {
        float m_time{ 0.f };
        float m_horizontalOffset{ 30.f };
        bool  m_moveLeft{ true };
        bool  m_started{ false };
        TransformComponent* m_transform{ nullptr };
        int   m_allEnemies{ 999 };
        int   m_dockedCount{ 0 };      // monotonically increasing — gates m_started
        int   m_inFormationCount{ 0 }; // ← tracks currently docked (rises/falls with dives)
        int   m_aliveEnemyCount{ 0 };
        float m_breathTime{};
        float m_breathSpeed{ 0.8f };
        float m_breathAmount{ 3.f };

        bool m_waitingForRegroup{ false };
        void LeftRight();
        void AdvanceBreath();

    public:
        FormationComponent(GameObject* owner);

        void Update() override;
        void HandleEvent(const Event* pEvent) override;

        void SetAllEnemies(int enemyCount);

        void NotifyDocked();
        void NotifyUndocked();         // ← new
        void NotifyDied();

        int  GetInFormationCount() const { return m_inFormationCount; } // useful for difficulty scaling later

        glm::vec3 ComputeSwayOffset(const glm::vec3& slotLocalPos, const glm::vec3& center) const;

        void TriggerPlayerDeathRegroup() { m_waitingForRegroup = true; }
        bool IsWaitingForRegroup() const { return m_waitingForRegroup; }
    };
}