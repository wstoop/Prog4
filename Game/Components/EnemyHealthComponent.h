#pragma once
#include "Components/Component.h"
#include "EventManager.h"
#include <functional>

namespace dae
{
    class EnemyHealthComponent : public Component, public EventHandler
    {
    public:
        using DeathCallback = std::function<void(GameObject*)>;
        using DamageFilter = std::function<bool(GameObject* self, GameObject* other)>;

        EnemyHealthComponent(GameObject* owner, float maxHealth);
        void HandleEvent(const Event* pEvent) override;

        void TakeDamage(float amount);
        void Kill();

        void RegisterDeathCallback(DeathCallback cb) { m_OnDeath = std::move(cb); }
        void SetDamageFilter(DamageFilter filter) { m_Filter = std::move(filter); }

        float GetHealth() const { return m_CurrentHealth; }
        float GetMaxHealth() const { return m_MaxHealth; }

    private:
        void CalculateDamage(const Event* pEvent);

        float m_MaxHealth;
        float m_CurrentHealth;

        DeathCallback m_OnDeath;
        DamageFilter  m_Filter;
    };
}
