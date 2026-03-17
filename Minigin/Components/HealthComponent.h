#pragma once
#include "Component.h"
#include "../EventManager.h"

namespace dae
{
    constexpr EventId ACTOR_DAMAGED = make_sdbm_hash("ActorDamaged");
    constexpr EventId ACTOR_DIED = make_sdbm_hash("ActorDied");

    struct DamageData
    {
        float amount;
        GameObject* target;
    };

    class HealthComponent final : public Component, public EventHandler
    {
    public:
        HealthComponent(GameObject* owner, float maxHealth = 100.f);
        ~HealthComponent() override = default;

        void HandleEvent(const Event* pEvent) override;

        void TakeDamage(float amount);

        float GetHealth() const { return m_CurrentHealth; }
        float GetMaxHealth() const { return m_MaxHealth; }
        bool  IsDead() const { return m_IsDead; }

    private:
        float m_MaxHealth;
        float m_CurrentHealth;
        bool  m_IsDead{ false };
    };
}