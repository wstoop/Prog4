#include "../GameObject.h"
#include "../EventManager.h"
#include "../GameEvents.h"
#include "BulletComponent.h"
#include "HitboxComponent.h"
#include "TransformComponent.h"
#include "EnemyDataComponent.h"
#include "AnimationComponent.h"
#include "HealthComponent.h"
#include "../Texture2D.h"
#include "../ExplosionPool.h"
#include <memory>

namespace dae
{
    HealthComponent::HealthComponent(GameObject* owner, float maxHealth)
        : Component(owner)
        , m_MaxHealth(maxHealth)
        , m_CurrentHealth(maxHealth)
    {
        EventManager::GetInstance().AttachEvent(ACTOR_DAMAGED, this);
        EventManager::GetInstance().AttachEvent(ACTOR_OVERLAPPED, this);
    }

    void HealthComponent::HandleEvent(const Event* pEvent)
    {
        if (pEvent->id == ACTOR_DAMAGED)
        {
            const auto* e = static_cast<const DataEvent<DamageData>*>(pEvent);
            if (e->data.target != GetOwner()) return;
            TakeDamage(e->data.amount);
        }
        else if (pEvent->id == ACTOR_OVERLAPPED)
        {
            const auto* e = static_cast<const DataEvent<OverlapData>*>(pEvent);
            if (e->data.self != GetOwner()) return;
            if (e->data.self->tag != "Enemy") return;
            if (e->data.other->tag != "Bullet") return;
            auto* shooter = e->data.other->GetComponent<BulletComponent>()->GetShooter();

            TakeDamage(1.f);
            e->data.other->GetComponent<BulletComponent>()->Deactivate();

            int score = 100; // fallback
            if (auto* data = GetOwner()->GetComponent<EnemyDataComponent>())
                score = data->GetScoreValue();

            EventManager::GetInstance().SendEvent(
                std::make_unique<DataEvent<ScoreEvent>>(
                    SCORE_CHANGED, ScoreEvent{ shooter, score }
                )
            );
        }

    }

    void HealthComponent::TakeDamage(float amount)
    {
        if (m_IsDead) return;

        m_CurrentHealth -= amount;
        if (m_CurrentHealth <= 0.f)
        {
            m_CurrentHealth = 0.f;
            m_IsDead = true;

            auto* anim = GetOwner()->GetComponent<AnimationComponent>();
            auto center = GetOwner()->GetComponent<TransformComponent>()->GetWorldPosition();
            center.x += (anim->GetSize().x / 2);
            center.y += (anim->GetSize().y / 2);
            ExplosionPool::GetInstance().ActivateExplosion(center);

            EventManager::GetInstance().SendEvent(ACTOR_DIED);
            GetOwner()->m_destroy = true;
        }
    }
}