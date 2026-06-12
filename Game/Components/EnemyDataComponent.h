#pragma once
#include <functional>
#include <memory>
#include "Components/Component.h"
#include "EventManager.h"
#include "GameEvents.h"
#include "Sound/ServiceLocator.h"
#include "../SoundID.h"

namespace dae
{
    class IEnemyState;
    class GameObject;

    using CombatStateFactory = std::function<std::unique_ptr<IEnemyState>()>;

    class EnemyDataComponent final : public Component, public EventHandler
    {
        int  m_scoreValue{ 0 };
        bool m_isBoss{ false };
        bool m_hit{ false };

		bool m_playerAlive{ true };

        GameObject* m_evilShip{ nullptr };

        sound_id m_hitSound{ SOUND_ENEMY_HIT };
        sound_id m_bossHit1{ SOUND_BOSS_HIT_1 };
        sound_id m_bossHit2{ SOUND_BOSS_HIT_2 };

        CombatStateFactory m_factory;

    public:
        EnemyDataComponent(GameObject* owner, int scoreValue)
            : Component(owner), m_scoreValue(scoreValue)
        {
            EventManager::GetInstance().AttachEvent(DATEVENT_ACTOR_OVERLAPPED, this);
			EventManager::GetInstance().AttachEvent(EVENT_PLAYER_TOOK_DAMAGE, this);
            EventManager::GetInstance().AttachEvent(EVENT_ALL_ENEMIES_RETURNED, this);
        }

        void SetBoss()
        {
            m_isBoss = true;
        }

        void SetHitSound(sound_id id) { m_hitSound = id; }
        void SetBossHitSounds(sound_id first, sound_id second)
        {
            m_isBoss = true;
            m_bossHit1 = first;
            m_bossHit2 = second;
        }

        sound_id GetHitSound()                        const { return m_hitSound; }
        sound_id GetBossHitSound(int remainingHp)     const
        {
            return remainingHp == 2 ? m_bossHit1 : m_bossHit2;
        }

        int  GetScoreValue() const { return m_scoreValue; }
        bool IsBoss()        const { return m_isBoss; }
        void HandleEvent(const Event* pEvent) override;

        void SetCombatStateFactory(CombatStateFactory f) { m_factory = std::move(f); }
        CombatStateFactory GetCombatStateFactory()                const { return m_factory; }

		bool IsPlayerAlive() const { return m_playerAlive; }

        void SetEvilShip(GameObject* evilShip) { m_evilShip = evilShip; }
        GameObject* GetEvilShip() const { return m_evilShip; }
    };
}