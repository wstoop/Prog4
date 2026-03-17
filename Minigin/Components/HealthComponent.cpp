#include "HealthComponent.h"
#include "BulletComponent.h"
#include "EnemyDataComponent.h"
#include "../GameObject.h"
#include "../EventManager.h"
#include "../GameEvents.h"
#include <memory>

namespace dae
{
    HealthComponent::HealthComponent(GameObject* owner, float maxHealth)
        : Component(owner)
        , m_MaxHealth(maxHealth)
        , m_CurrentHealth(maxHealth)
    {
        EventManager::GetInstance().AttachEvent(ACTOR_OVERLAPPED, this);
    }

    void HealthComponent::HandleEvent(const Event* pEvent)
    {
        if (pEvent->id != ACTOR_OVERLAPPED) return;

        const auto* e = static_cast<const DataEvent<OverlapEvent>*>(pEvent);
        if (e->data.self != GetOwner()) return;
        if (m_Filter && !m_Filter(e->data.self, e->data.other)) return;

        TakeDamage(1.f);
        auto* bullet = e->data.other->GetComponent<BulletComponent>();
        if (bullet == nullptr) return;
        bullet->Deactivate();

        if (auto* data = GetOwner()->GetComponent<EnemyDataComponent>())
        {
            EventManager::GetInstance().SendEvent(
                std::make_unique<DataEvent<ScoreEvent>>(
                    SCORE_CHANGED, ScoreEvent{ bullet->GetShooter(), data->GetScoreValue() }
                )
            );
        }
    }

    void HealthComponent::TakeDamage(float amount)
    {
        if (m_IsDead) return;

        m_CurrentHealth -= amount;
        if (m_CurrentHealth > 0.f) return;

        m_CurrentHealth = 0.f;
        m_IsDead = true;

        if (m_OnDeath)
            m_OnDeath(GetOwner());
    }

    void HealthComponent::Revive()
    {
        m_CurrentHealth = m_MaxHealth;
        m_IsDead = false;
    }
}