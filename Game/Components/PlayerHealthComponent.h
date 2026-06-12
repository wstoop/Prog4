#pragma once
#include "Components/Component.h"
#include "EventManager.h"
#include <functional>

namespace dae
{
    class PlayerHealthComponent : public Component, public EventHandler
    {
    public:
        using DeathCallback = std::function<void(GameObject*)>;
        using DamageFilter = std::function<bool(GameObject* self, GameObject* other)>;

        PlayerHealthComponent(GameObject* owner, float maxHealth);
        void HandleEvent(const Event* pEvent) override;
        void Update() override;

        bool TakeDamage(float amount);

        void Capture();

        void Revive();

        void RegisterDeathCallback(DeathCallback cb) { m_OnDeath = std::move(cb); }
        void SetDamageFilter(DamageFilter filter) { m_Filter = std::move(filter); }

        float GetHealth() const { return m_CurrentHealth; }
        float GetMaxHealth() const { return m_MaxHealth; }
        bool  IsDead() const { return m_IsDead; }

    private:
        void CalculateDamage(const Event* pEvent);
        void Die();
        void TryRespawn();

        float m_MaxHealth;
        float m_CurrentHealth;
        bool  m_IsDead{ false };

        DeathCallback m_OnDeath;
        DamageFilter  m_Filter;

        float m_InvincibilityTimer{ 0.f };
        static constexpr float k_InvincibilityDuration{ 1.5f };

        float m_RespawnTimer{ -1.f };
        static constexpr float k_RespawnDelay{ 2.5f };
    };
}
