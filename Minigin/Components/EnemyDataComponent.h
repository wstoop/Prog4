#pragma once
#include "Component.h"
#include "../EventManager.h"
#include "../GameEvents.h"

namespace dae
{
    class EnemyDataComponent final : public Component, public EventHandler
    {
        int m_scoreValue{ 0 };
        bool m_isBoss{ false };
        bool m_hit{ false };

    public:
        EnemyDataComponent(GameObject* owner, int scoreValue)
            : Component(owner), m_scoreValue(scoreValue) {
        }

        void SetBoss()
        {
            m_isBoss = true;
            EventManager::GetInstance().AttachEvent(ACTOR_OVERLAPPED, this);
        }

        int GetScoreValue() const { return m_scoreValue; }
        void HandleEvent(const Event* pEvent) override;
    };
}