#include "EnemyFactory.h"

#include "Components/AnimationComponent.h"
#include "Components/HitboxComponent.h"
#include "Components/HealthComponent.h"
#include "Components/EnemyDataComponent.h"
#include "Components/BulletComponent.h"
#include "Components/TransformComponent.h"
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
            e->AddComponent<dae::HitboxComponent>(13.f * 3, 11.f * 3);
            e->AddComponent<dae::EnemyDataComponent>(50);
            auto* hp = e->AddComponent<dae::HealthComponent>(1.f);
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
            e->AddComponent<dae::HitboxComponent>(13.f * 3, 10.f * 3);
            e->AddComponent<dae::EnemyDataComponent>(80);
            auto* hp = e->AddComponent<dae::HealthComponent>(1.f);
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
            e->AddComponent<dae::HitboxComponent>(15.f * 3, 16.f * 3);
            e->AddComponent<dae::EnemyDataComponent>(150);
            e->GetComponent<dae::EnemyDataComponent>()->SetBoss();
            auto* hp = e->AddComponent<dae::HealthComponent>(2.f);
            hp->SetDamageFilter(DamageFilter);
            hp->RegisterDeathCallback(DeathCallback);
            e->GetComponent<dae::TransformComponent>()->SetLocalPosition({ -100, -100, 0 });
            return e;
        });
}