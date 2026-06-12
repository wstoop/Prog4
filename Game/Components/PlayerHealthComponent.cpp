#include "PlayerHealthComponent.h"
#include "BulletComponent.h"
#include "GameObject.h"
#include "EventManager.h"
#include "GameEvents.h"
#include "Components/TransformComponent.h"
#include "TimeManager.h"
#include "Scene.h"
#include "SceneManager.h"
#include <memory>

namespace dae
{
    PlayerHealthComponent::PlayerHealthComponent(GameObject* owner, float maxHealth)
        : Component(owner)
        , m_MaxHealth(maxHealth)
        , m_CurrentHealth(maxHealth)
    {
        EventManager::GetInstance().AttachEvent(DATEVENT_ACTOR_OVERLAPPED, this);
        EventManager::GetInstance().AttachEvent(EVENT_ALL_ENEMIES_RETURNED, this);
    }

    void PlayerHealthComponent::HandleEvent(const Event* pEvent)
    {
        switch (pEvent->id)
        {
        case DATEVENT_ACTOR_OVERLAPPED:
            CalculateDamage(pEvent);
            break;

        case EVENT_ALL_ENEMIES_RETURNED:
            TryRespawn();
            break;
        }
    }

    bool PlayerHealthComponent::TakeDamage(float amount)
    {
        if (m_InvincibilityTimer > 0.f) return false;

        m_CurrentHealth -= amount;
        m_InvincibilityTimer = k_InvincibilityDuration;

        if (m_CurrentHealth > 0.f) return true;

        m_CurrentHealth = 0.f;
        Die();
        return true;
    }

    void PlayerHealthComponent::Capture()
    {
        if (m_IsDead) return;

        m_CurrentHealth = 0.f;
        Die();
    }

    void PlayerHealthComponent::Die()
    {
        m_IsDead = true;
        m_RespawnTimer = k_RespawnDelay;
        GetOwner()->GetComponent<TransformComponent>()->SetLocalPosition({ -100.f, -100.f, 0.f });

        bool otherPlayerAlive = false;
        if (auto* scene = SceneManager::GetInstance().GetActiveScene())
        {
            for (const auto& obj : scene->GetObjects())
            {
                if (obj.get() == GetOwner()) continue;
                if (obj->tag != "Player") continue;

                auto* otherHealth = obj->GetComponent<PlayerHealthComponent>();
                if (otherHealth && !otherHealth->IsDead())
                {
                    otherPlayerAlive = true;
                    break;
                }
            }
        }

        EventManager::GetInstance().SendEvent(
            std::make_unique<DataEvent<PlayerTookDamageEvent>>(
                EVENT_PLAYER_TOOK_DAMAGE,
                PlayerTookDamageEvent{ otherPlayerAlive }
            )
        );
    }

    void PlayerHealthComponent::Revive()
    {
        m_CurrentHealth = m_MaxHealth;
        m_IsDead = false;
    }

    void PlayerHealthComponent::TryRespawn()
    {
        m_RespawnTimer = -1.f;
        if (m_IsDead && m_OnDeath)
            m_OnDeath(GetOwner());
    }

    void PlayerHealthComponent::CalculateDamage(const Event* pEvent)
    {
        const auto* e = static_cast<const DataEvent<OverlapEvent>*>(pEvent);

        if (e->data.self != GetOwner()) return;
        if (m_Filter && !m_Filter(e->data.self, e->data.other)) return;

        TakeDamage(1.f);

        if (auto* bullet = e->data.other->GetComponent<BulletComponent>())
            bullet->Deactivate();
    }

    void PlayerHealthComponent::Update()
    {
        if (m_InvincibilityTimer > 0.f)
            m_InvincibilityTimer -= TimeManager::GetInstance().GetDeltaTime();

        if (m_RespawnTimer > 0.f)
        {
            m_RespawnTimer -= TimeManager::GetInstance().GetDeltaTime();
            if (m_RespawnTimer <= 0.f)
                TryRespawn();
        }
    }
}
