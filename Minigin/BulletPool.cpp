#include "BulletPool.h"
#include "GameObject.h"
#include "Components/BulletComponent.h"
#include "Components/TextureComponent.h"
#include "Components/TransformComponent.h"
#include "Components/HitboxComponent.h"
#include <memory>
#include <algorithm>
#include <iterator>

namespace dae
{
    std::unique_ptr<GameObject> BulletPool::CreateBullet() const
    {
        auto bullet = std::make_unique<GameObject>();
        bullet->tag = "Bullet";
        bullet->AddComponent<BulletComponent>();
        bullet->AddComponent<TextureComponent>("bullet.png");
		bullet->AddComponent<HitboxComponent>(3.f * 3, 8.f * 3);
        bullet->GetComponent<TransformComponent>()->SetScale({ 3.f, 3.f, 3.f });
        bullet->GetComponent<BulletComponent>()->SetActive(false);
        return bullet;
    }

    GameObject* BulletPool::GetBullet()
    {
        auto it = std::find_if(m_bullets.begin(), m_bullets.end(), [](const std::unique_ptr<GameObject>& bullet)
            {
                return !bullet->GetComponent<BulletComponent>()->IsActive();
            });

        if (it == m_bullets.end())
        {
            m_bullets.emplace_back(CreateBullet());
            it = std::prev(m_bullets.end());
        }

        (*it)->GetComponent<BulletComponent>()->SetActive(true);
        return it->get();
    }

    void BulletPool::ReturnBullet(GameObject* bullet)
    {
        if (!bullet) return;
        bullet->GetComponent<BulletComponent>()->SetActive(false);
    }

    void BulletPool::Update()
    {

        std::for_each(m_bullets.begin(), m_bullets.end(), [](const std::unique_ptr<GameObject>& bullet)
            {
                if (bullet->GetComponent<BulletComponent>()->IsActive())
                    bullet->Update();
            });
    }

    void BulletPool::Render() const
    {
        std::for_each(m_bullets.begin(), m_bullets.end(), [](const std::unique_ptr<GameObject>& bullet)
            {
                if (bullet->GetComponent<BulletComponent>()->IsActive())
                    bullet->Render();
            });
    }
}