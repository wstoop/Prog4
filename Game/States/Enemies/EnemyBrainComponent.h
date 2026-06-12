#pragma once
#include "Components/Component.h"
#include "IEnemyState.h"
#include <memory>
#include <glm/glm.hpp>

namespace dae {

    class EnemyBrainComponent final : public Component {
    public:
        EnemyBrainComponent(GameObject* owner);
        void Update() override;

        void TransitionTo(std::unique_ptr<IEnemyState> next);

        // Sends the enemy back into formation via a top-entry loop (EnteringState),
        // targeting its formation slot. Used when dive/attack states finish.
        void ReturnToFormationViaEntry();

        // Formation parent
        GameObject* GetFormationParent() const { return m_formationParent; }
        void             SetFormationParent(GameObject* p) { m_formationParent = p; }

        // Entry config — read by EnteringState::OnEnter to build the path
        const glm::vec3& GetEntryTarget()   const { return m_entryTarget; }
        float            GetEntryDuration() const { return m_entryDuration; }
        float            GetEntryDelay()    const { return m_entryDelay; }
        bool             GetEntryFromLeft() const { return m_entryFromLeft; }

        void SetEntryConfig(const glm::vec3& target, float duration, float delay,
            bool fromLeft, GameObject* formationParent)
        {
            m_entryTarget = target;
            m_entryDuration = duration;
            m_entryDelay = delay;
            m_entryFromLeft = fromLeft;
            m_formationParent = formationParent;
            m_entryConfigSet = true;
        }

        // Score multiplier: 1 in formation, >1 during dive
        int              GetScoreMultiplier() const { return m_scoreMultiplier; }
        void             SetScoreMultiplier(int m) { m_scoreMultiplier = m; }

        // Hit flag — used by BossState
        bool             IsHit() const { return m_isHit; }
        void             MarkHit();

    private:
        std::unique_ptr<IEnemyState> m_currentState;

        // Entry config
        glm::vec3  m_entryTarget{};
        float      m_entryDuration{ 2.f };
        float      m_entryDelay{ 0.f };
        bool       m_entryFromLeft{ true };
        bool       m_entryConfigSet{ false };
        bool       m_entered{ false };

        GameObject* m_formationParent{ nullptr };
        int  m_scoreMultiplier{ 1 };
        bool m_isHit{ false };
    };

}