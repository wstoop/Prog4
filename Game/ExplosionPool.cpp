#include "ExplosionPool.h"
#include "GameObject.h"
#include "Components/AnimationComponent.h"
#include "Components/TransformComponent.h"
#include <memory>
#include <algorithm>

void dae::ExplosionPool::ActivateExplosion(const glm::vec3 & enemyCenter)
{
    auto it = std::find_if(m_explosions.begin(), m_explosions.end(), [](const std::unique_ptr<dae::GameObject>& e) {
        auto* anim = e->GetComponent<dae::AnimationComponent>();
        return anim && !anim->IsPlaying();
        });

    dae::GameObject* explosion = (it == m_explosions.end())
        ? m_explosions.emplace_back(CreateExplosion()).get()
        : it->get();

    auto* anim = explosion->GetComponent<dae::AnimationComponent>();
    auto* transform = explosion->GetComponent<dae::TransformComponent>();

    if (anim && transform)
    {
        glm::vec3 finalPos{};
        finalPos.x = enemyCenter.x - (anim->GetSize().x / 2);
        finalPos.y = enemyCenter.y - (anim->GetSize().y / 2);

        transform->SetLocalPosition(finalPos);
        anim->Play();
    }
}

std::unique_ptr<dae::GameObject> dae::ExplosionPool::CreateExplosion() const
{
    auto explosion = std::make_unique<dae::GameObject>();

    explosion->AddComponent<dae::AnimationComponent>("explosion.png", 5, 1, 0.08f, false);

    if (auto* transform = explosion->GetComponent<dae::TransformComponent>())
    {
        transform->SetScale({ 3.f, 3.f, 3.f });
    }

    return explosion;
}

void dae::ExplosionPool::Update()
{
    for (auto& explosion : m_explosions)
    {
        dae::AnimationComponent* anim = explosion->GetComponent<dae::AnimationComponent>();
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
        dae::AnimationComponent* anim = explosion->GetComponent<dae::AnimationComponent>();
        if (anim && anim->IsPlaying())
        {
            explosion->Render();
        }
    }
}