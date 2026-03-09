#include "BulletPool.h"
#include "GameObject.h"
#include "Components/BulletComponent.h"
#include "Components/TextureComponent.h"
#include "Components/TransformComponent.h"
#include "Scene.h"

namespace dae
{
    void BulletPool::Initialize(dae::Scene* scene, int poolSize)
    {
        m_bullets.reserve(poolSize);

        for (int i = 0; i < poolSize; ++i)
        {
            auto bullet = std::make_unique<GameObject>();
            bullet->AddComponent<BulletComponent>();
            bullet->AddComponent<TextureComponent>("bullet.png");
			bullet->GetComponent<TransformComponent>()->SetScale({ 3.f, 3.f, 3.f });
            bullet->GetComponent<dae::TransformComponent>()
                ->SetLocalPosition({ -300.f, -800.f, 0.f });
            GameObject* rawPtr = bullet.get();
            scene->Add(std::move(bullet));
            m_bullets.emplace_back(rawPtr, false);
        }
    }

    GameObject* BulletPool::GetBullet()
    {
        for (auto& [bullet, inUse] : m_bullets)
        {
            if (!inUse)
            {
                inUse = true;
                return bullet;
            }
        }

        // Pool exhausted
        return nullptr;
    }

    void BulletPool::ReturnBullet(GameObject* bullet)
    {
        if (bullet == nullptr) return;

        for (auto& [poolBullet, inUse] : m_bullets)
        {
            if (poolBullet == bullet)
            {
                inUse = false;
                return;
            }
        }
    }
}