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
        int   m_maxConcurrentAttackers{ 2 };
        float m_breathTime{};
        float m_breathSpeed{ 0.8f };
        float m_breathAmount{ 3.f };

        bool m_waitingForRegroup{ false };

        // The player-controlled boss in Versus has its own FormationComponent
        // (so it can dock/undock for its attack states) but isn't part of the
        // WaveManager's wave - its death shouldn't trigger EVENT_WAVE_CLEARED
        // and tear down the actual enemy formation.
        bool m_reportsWaveCleared{ true };

        void LeftRight();
        void AdvanceBreath();

    public:
        FormationComponent(GameObject* owner);

        void Update() override;
        void HandleEvent(const Event* pEvent) override;

        void SetAllEnemies(int enemyCount);
        void SetReportsWaveCleared(bool reports) { m_reportsWaveCleared = reports; }

        void NotifyDocked();
        void NotifyUndocked();         // ← new
        void NotifyDied(bool wasInFormation);

        int  GetInFormationCount() const { return m_inFormationCount; } // useful for difficulty scaling later

        bool CanStartAttack() const { return (m_aliveEnemyCount - m_inFormationCount) < m_maxConcurrentAttackers; }

        glm::vec3 ComputeSwayOffset(const glm::vec3& slotLocalPos, const glm::vec3& center) const;

        void TriggerPlayerDeathRegroup() { m_waitingForRegroup = true; }
        bool IsWaitingForRegroup() const { return m_waitingForRegroup; }
    };
}