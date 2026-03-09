#include "ShootComponent.h"
#include "../BulletPool.h"
#include "../GameObject.h"
#include "TransformComponent.h"
#include "BulletComponent.h"

namespace dae
{
    ShootComponent::ShootComponent(GameObject* owner, float speed)
        : Component(owner)
        , m_speed{ speed }
    {
        m_transform = GetOwner()->GetComponent<TransformComponent>();
    }

    void ShootComponent::Shoot(float dirX, float dirY)
    {
        GameObject* bullet = BulletPool::GetInstance().GetBullet();
        if (bullet == nullptr) return;
        const auto& pos = m_transform->GetLocalPosition();

        auto* bulletComp = bullet->GetComponent<BulletComponent>();
        if (bulletComp == nullptr) return;

        bulletComp->Activate(pos.x, pos.y, dirX, dirY, m_speed);
    }
}