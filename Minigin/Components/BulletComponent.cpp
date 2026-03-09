#include "BulletComponent.h"
#include "BulletPool.h"
#include "GameObject.h"
#include "Components/TransformComponent.h"
#include "../TimeManager.h"

namespace dae
{
    BulletComponent::BulletComponent(GameObject* owner)
        : Component(owner)
        , m_transform{ owner->GetComponent<TransformComponent>() }
    {
    }

    void BulletComponent::Activate(float x, float y, float dirX, float dirY, float speed)
    {
        m_dirX = dirX;
        m_dirY = dirY;
        m_speed = speed;
        m_active = true;

        m_transform->SetLocalPosition(glm::vec3{x, y, 0});
    }

    void BulletComponent::Deactivate()
    {
		m_transform->SetLocalPosition(glm::vec3{ -300.f, -800.f, 0.f });
        m_active = false;
        BulletPool::GetInstance().ReturnBullet(GetOwner());
    }

    void BulletComponent::Update()
    {
        if (!m_active) return;

		float delta = dae::TimeManager::GetInstance().GetDeltaTime();
        const auto& pos = m_transform->GetLocalPosition();
		float moveX = pos.x + m_dirX * m_speed * delta;
		float moveY = pos.y + m_dirY * m_speed * delta;
        m_transform->SetLocalPosition(glm::vec3{moveX, moveY, 0});

		if (pos.y < -10)
        {
            Deactivate();
        }
    }
}