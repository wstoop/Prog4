#include "EnemyFactory.h"

#include "Components/AnimationComponent.h"
#include "Components/HitboxComponent.h"
#include "Components/EnemyHealthComponent.h"
#include "Components/EnemyDataComponent.h"
#include "Components/TractorBeamComponent.h"
#include "Components/BulletComponent.h"
#include "Components/TransformComponent.h"
#include "Components/ShootComponent.h"
#include "States/Enemies/Bee/BeeDiveBombState.h"
#include "States/Enemies/Butterfly/ButterflyDiveBombState.h"
#include "States/Enemies/Boss/BossAttackDecideState.h"
#include "ExplosionPool.h"
#include "GameObject.h"

std::unordered_map<char, dae::EnemyFactory::Creator> dae::EnemyFactory::m_Creators;
bool dae::EnemyFactory::s_defaultsRegistered = false;
void dae::EnemyFactory::Register(char type, Creator creator)
{
    m_Creators[type] = creator;
}

std::unique_ptr<dae::GameObject> dae::EnemyFactory::Create(char type)
{
    auto it = m_Creators.find(type);
    if (it != m_Creators.end())
        return it->second();

    return nullptr;
}

void dae::EnemyFactory::RegisterDefaults()
{
    if (s_defaultsRegistered) return;
    s_defaultsRegistered = true;

    //Shared callbacks
    static auto DeathCallback = [](dae::GameObject* self)
        {
            auto* anim = self->GetComponent<dae::AnimationComponent>();
            auto* trans = self->GetComponent<dae::TransformComponent>();
            auto  center = trans->GetWorldPosition();
            center.x += anim->GetSize().x / 2.f;
            center.y += anim->GetSize().y / 2.f;
            ExplosionPool::GetInstance().ActivateExplosion(center);

            if (auto* data = self->GetComponent<dae::EnemyDataComponent>())
            {
                if (auto* evilShip = data->GetEvilShip())
                    evilShip->m_destroy = true;
            }

            self->m_destroy = true;
        };

    static auto DamageFilter = [](dae::GameObject* /*self*/, dae::GameObject* other) -> bool
        {
            if (other->tag != "Bullet") return false;
            return other->GetComponent<dae::BulletComponent>()->GetShooter()->tag == "Player";
        };

    //Bee
    Register('B', []()
        {
            auto e = std::make_unique<dae::GameObject>();
            e->AddComponent<dae::AnimationComponent>("beeIdle.png", 2, 1, 0.2f);
            e->AddComponent<dae::ShootComponent>(600.f);
            e->AddComponent<dae::HitboxComponent>(13.f * 3, 11.f * 3);
            e->AddComponent<dae::EnemyDataComponent>(50);
            e->GetComponent<EnemyDataComponent>()->SetHitSound(SOUND_ENEMY_HIT);
            e->GetComponent<EnemyDataComponent>()->SetCombatStateFactory([] { return std::make_unique<BeeDiveBombState>(); });
            auto* hp = e->AddComponent<dae::EnemyHealthComponent>(1.f);
            hp->SetDamageFilter(DamageFilter);
            hp->RegisterDeathCallback(DeathCallback);
            e->GetComponent<dae::TransformComponent>()->SetLocalPosition({ -100, -100, 0 });
            return e;
        });

    //Butterfly
    Register('W', []()
        {
            auto e = std::make_unique<dae::GameObject>();
            e->AddComponent<dae::AnimationComponent>("butterflyIdle.png", 2, 1, 0.2f);
            e->AddComponent<dae::ShootComponent>(600.f);
            e->AddComponent<dae::HitboxComponent>(13.f * 3, 10.f * 3);
            e->AddComponent<dae::EnemyDataComponent>(80);
            e->GetComponent<EnemyDataComponent>()->SetHitSound(SOUND_ENEMY_HIT);
            e->GetComponent<EnemyDataComponent>()->SetCombatStateFactory([] { return std::make_unique<ButterflyDiveBombState>(); });
            auto* hp = e->AddComponent<dae::EnemyHealthComponent>(1.f);
            hp->SetDamageFilter(DamageFilter);
            hp->RegisterDeathCallback(DeathCallback);
            e->GetComponent<dae::TransformComponent>()->SetLocalPosition({ -100, -100, 0 });
            return e;
        });

    //Boss
    Register('G', []()
        {
            auto e = std::make_unique<dae::GameObject>();
            e->AddComponent<dae::AnimationComponent>("birdIdle.png", 2, 2, 0.2f);
            e->AddComponent<dae::ShootComponent>(600.f);
            e->AddComponent<dae::HitboxComponent>(15.f * 3, 16.f * 3);
            e->AddComponent<dae::EnemyDataComponent>(150);
            e->GetComponent<EnemyDataComponent>()->SetBossHitSounds(SOUND_BOSS_HIT_1, SOUND_BOSS_HIT_2);
            e->GetComponent<EnemyDataComponent>()->SetHitSound(SOUND_ENEMY_HIT);
            e->GetComponent<EnemyDataComponent>()->SetCombatStateFactory([] { return std::make_unique<BossAttackDecideState>(); });
            e->GetComponent<dae::EnemyDataComponent>()->SetBoss();
            e->AddComponent<dae::TractorBeamComponent>();
            auto* hp = e->AddComponent<dae::EnemyHealthComponent>(2.f);
            hp->SetDamageFilter(DamageFilter);
            hp->RegisterDeathCallback(DeathCallback);
            e->GetComponent<dae::TransformComponent>()->SetLocalPosition({ -100, -100, 0 });
            return e;
        });
}