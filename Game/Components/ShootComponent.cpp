#include "ShootComponent.h"
#include "BulletPool.h"
#include "GameObject.h"
#include "Components/TransformComponent.h"
#include "Components/TextureComponent.h"
#include "Texture2D.h"
#include "Components/BulletComponent.h"
#include "Sound/ServiceLocator.h"
#include "../SoundID.h"


namespace dae
{
    ShootComponent::ShootComponent(GameObject* owner, float speed)
        : Component(owner)
        , m_speed{ speed }
    {
        m_transform = GetOwner()->GetComponent<TransformComponent>();
		m_sprite = GetOwner()->GetComponent<TextureComponent>();
    }

    void ShootComponent::Shoot(float dirX, float dirY)
    {
        ServiceLocator::GetSoundSystem().Play(SOUND_SHOOT, 0.8f);
        GameObject* bullet = BulletPool::GetInstance().GetBullet();
        if (bullet == nullptr) return;
        const auto& pos = m_transform->GetLocalPosition();
        auto* bulletComp = bullet->GetComponent<BulletComponent>();
        if (bulletComp == nullptr) return;

        float spriteHalfW = 0.f;
        float spriteHalfH = 0.f;
        float bulletHalfW = 0.f;
        float bulletHalfH = 0.f;

        if (m_sprite && m_sprite->GetTexture())
        {
            spriteHalfW = (m_sprite->GetTexture()->GetSize().x) * 0.5f;
            spriteHalfH = (m_sprite->GetTexture()->GetSize().y) * 0.5f;
        }

        auto* bulletTransform = bullet->GetComponent<TransformComponent>();
        auto* bulletSprite = bullet->GetComponent<TextureComponent>();
        if (bulletSprite && bulletSprite->GetTexture() && bulletTransform)
        {
            bulletHalfW = (bulletSprite->GetTexture()->GetSize().x) * 0.5f;
            bulletHalfH = (bulletSprite->GetTexture()->GetSize().y) * 0.5f;
        }

        bulletComp->Activate(GetOwner(),
            pos.x + spriteHalfW - bulletHalfW,
            pos.y + spriteHalfH - bulletHalfH,
            dirX, dirY, m_speed);
    }
}