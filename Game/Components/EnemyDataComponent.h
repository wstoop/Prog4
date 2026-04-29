#pragma once
#include "Components/Component.h"
#include "EventManager.h"
#include "GameEvents.h"
#include "Sound/ServiceLocator.h"
#include "../SoundID.h"
namespace dae
{
    class EnemyDataComponent final : public Component, public EventHandler
    {
        int m_scoreValue{ 0 };
        bool m_isBoss{ false };
        bool m_hit{ false };

        sound_id m_hitSound{ SOUND_ENEMY_HIT };
        sound_id m_bossHit1{ SOUND_BOSS_HIT_1 };
        sound_id m_bossHit2{ SOUND_BOSS_HIT_2 };

    public:
        EnemyDataComponent(GameObject* owner, int scoreValue)
            : Component(owner), m_scoreValue(scoreValue) {
        }

        void SetBoss()
        {
            m_isBoss = true;
            EventManager::GetInstance().AttachEvent(ACTOR_OVERLAPPED, this);
        }

        void SetHitSound(sound_id id) { m_hitSound = id; }
        void SetBossHitSounds(sound_id first, sound_id second)
        {
            m_isBoss = true;
            m_bossHit1 = first;
            m_bossHit2 = second;
            EventManager::GetInstance().AttachEvent(ACTOR_OVERLAPPED, this);
        }

        sound_id GetHitSound() const { return m_hitSound; }
        sound_id GetBossHitSound(int remainingHp) const
        {
            return remainingHp == 2 ? m_bossHit1 : m_bossHit2;
        }

        int GetScoreValue() const { return m_scoreValue; }
        bool IsBoss() const { return m_isBoss; }
        void HandleEvent(const Event* pEvent) override;
    };
}