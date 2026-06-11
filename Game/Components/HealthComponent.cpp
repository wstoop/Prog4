#include "HealthComponent.h"
#include "BulletComponent.h"
#include "EnemyDataComponent.h"
#include "GameObject.h"
#include "EventManager.h"
#include "GameEvents.h"
#include "LivesComponent.h"
#include "Components/TransformComponent.h"
#include "TimeManager.h"
#include <memory>

namespace dae
{
    HealthComponent::HealthComponent(GameObject* owner, float maxHealth)
        : Component(owner)
        , m_MaxHealth(maxHealth)
        , m_CurrentHealth(maxHealth)
    {
        EventManager::GetInstance().AttachEvent(DATEVENT_ACTOR_OVERLAPPED, this);
        EventManager::GetInstance().AttachEvent(EVENT_ALL_ENEMIES_RETURNED, this);
    }

    void HealthComponent::HandleEvent(const Event* pEvent)
    {
        switch (pEvent->id)
        {
        case DATEVENT_ACTOR_OVERLAPPED:
            CalculateDamage(pEvent);
            break;

        case EVENT_ALL_ENEMIES_RETURNED:
            if (m_IsDead  && m_OnDeath && GetOwner()->GetComponent<LivesComponent>() != nullptr)
                m_OnDeath(GetOwner());
            break;
        }
    }

    void HealthComponent::TakeDamage(float amount)
    {
        if (m_IsDead) return;
        if (m_InvincibilityTimer > 0.f) return;

        m_CurrentHealth -= amount;
        m_InvincibilityTimer = k_InvincibilityDuration;

        if (m_CurrentHealth > 0.f) return;

        m_CurrentHealth = 0.f;
        m_IsDead = true;

		if (m_OnDeath && GetOwner()->GetComponent<EnemyDataComponent>() != nullptr)
			m_OnDeath(GetOwner());
    }

    void HealthComponent::Revive()
    {
        m_CurrentHealth = m_MaxHealth;
        m_IsDead = false;
    }

    void HealthComponent::CalculateDamage(const Event* pEvent)
    {
        if (pEvent->id != DATEVENT_ACTOR_OVERLAPPED) return;

        const auto* e = static_cast<const DataEvent<OverlapEvent>*>(pEvent);

        if (e->data.self != GetOwner()) return;
        if (m_Filter && !m_Filter(e->data.self, e->data.other)) return;

        auto* data = GetOwner()->GetComponent<EnemyDataComponent>();
        float beforeHp = m_CurrentHealth;

        if (data && !m_IsDead)
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

        if (GetOwner()->GetComponent<LivesComponent>() != nullptr)
        {
            EventManager::GetInstance().SendEvent(EVENT_PLAYER_TOOK_DAMAGE);
            GetOwner()->GetComponent<TransformComponent>()->SetLocalPosition({ -100.f, -100.f, 0.f });
        }

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

    void HealthComponent::Update()
    {
        if (m_InvincibilityTimer > 0.f)
            m_InvincibilityTimer -= TimeManager::GetInstance().GetDeltaTime();
    }

}