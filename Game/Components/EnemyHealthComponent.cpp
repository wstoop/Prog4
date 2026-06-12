#include "EnemyHealthComponent.h"
#include "BulletComponent.h"
#include "EnemyDataComponent.h"
#include "GameObject.h"
#include "EventManager.h"
#include "GameEvents.h"
#include <memory>

namespace dae
{
    EnemyHealthComponent::EnemyHealthComponent(GameObject* owner, float maxHealth)
        : Component(owner)
        , m_MaxHealth(maxHealth)
        , m_CurrentHealth(maxHealth)
    {
        EventManager::GetInstance().AttachEvent(DATEVENT_ACTOR_OVERLAPPED, this);
    }

    void EnemyHealthComponent::HandleEvent(const Event* pEvent)
    {
        if (pEvent->id != DATEVENT_ACTOR_OVERLAPPED) return;
        CalculateDamage(pEvent);
    }

    void EnemyHealthComponent::TakeDamage(float amount)
    {
        m_CurrentHealth -= amount;
        if (m_CurrentHealth > 0.f) return;

        m_CurrentHealth = 0.f;
        if (m_OnDeath) m_OnDeath(GetOwner());
    }

    void EnemyHealthComponent::Kill()
    {
        m_CurrentHealth = 0.f;
        if (m_OnDeath) m_OnDeath(GetOwner());
    }

    void EnemyHealthComponent::CalculateDamage(const Event* pEvent)
    {
        const auto* e = static_cast<const DataEvent<OverlapEvent>*>(pEvent);

        if (e->data.self != GetOwner()) return;
        if (m_Filter && !m_Filter(e->data.self, e->data.other)) return;

        auto* data = GetOwner()->GetComponent<EnemyDataComponent>();
        const float beforeHp = m_CurrentHealth;

        if (data)
        {
            if (data->IsBoss())
            {
                ServiceLocator::GetSoundSystem().Play(
                    data->GetBossHitSound(static_cast<int>(beforeHp)),
                    1.0f
                );
            }
            else
            {
                ServiceLocator::GetSoundSystem().Play(
                    data->GetHitSound(),
                    1.0f
                );
            }
        }

        TakeDamage(1.f);

        auto* bullet = e->data.other->GetComponent<BulletComponent>();
        if (bullet == nullptr) return;

        bullet->Deactivate();

        if (bullet->GetShooter()->GetComponent<EnemyDataComponent>() == nullptr)
        {
            EventManager::GetInstance().SendEvent(EVENT_PLAYER_DAMAGED_ENEMY);
        }

        if (data)
        {
            EventManager::GetInstance().SendEvent(
                std::make_unique<DataEvent<ScoreEvent>>(
                    DATEVENT_SCORE_CHANGED,
                    ScoreEvent{ bullet->GetShooter(), data->GetScoreValue() }
                )
            );
        }
    }
}
