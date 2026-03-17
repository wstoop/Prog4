#include "ExplosionPool.h"
#include "GameObject.h"
#include "Components/AnimationComponent.h"
#include "Components/TransformComponent.h"
#include <memory>
#include <algorithm>
#include <iterator>

namespace dae
{

    void dae::ExplosionPool::ActivateExplosion(const glm::vec3& enemyCenter)
    {
        auto it = std::find_if(m_explosions.begin(), m_explosions.end(), [](const std::unique_ptr<GameObject>& e) {
            return !e->GetComponent<AnimationComponent>()->IsPlaying();
            });

        GameObject* explosion = (it == m_explosions.end()) ? m_explosions.emplace_back(CreateExplosion()).get() : it->get();

        auto* anim = explosion->GetComponent<AnimationComponent>();
        glm::vec3 finalPos{};
        finalPos.x = enemyCenter.x - (anim->GetSize().x / 10);
        finalPos.y = enemyCenter.y - (anim->GetSize().y / 2);
        explosion->GetComponent<TransformComponent>()->SetLocalPosition(finalPos);
        anim->Play();
    }

    std::unique_ptr<GameObject> ExplosionPool::CreateExplosion() const
    {
        auto explosion = std::make_unique<GameObject>();
        explosion->AddComponent<AnimationComponent>("explosion.png", 5, 1, 0.03f, false);
        explosion->GetComponent<TransformComponent>()->SetScale({ 3.f, 3.f, 3.f });
        return explosion;
    }

    void dae::ExplosionPool::Update()
    {
        for (auto& explosion : m_explosions)
        {
            auto anim = explosion->GetComponent<AnimationComponent>();
            if (anim && anim->IsPlaying())
            {
                explosion->Update();
            }
        }
    }

    void dae::ExplosionPool::Render() const
    {
        for (const auto& explosion : m_explosions)
        {
            auto anim = explosion->GetComponent<AnimationComponent>();
            if (anim && anim->IsPlaying())
            {
                explosion->Render();
            }
        }
    }
}