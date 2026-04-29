#include "HitboxComponent.h"
#include "GameObject.h"
#include "GameEvents.h"
#include "Components/TransformComponent.h"

namespace dae
{
    std::vector<HitboxComponent*> HitboxComponent::s_allHitboxes{};

    HitboxComponent::HitboxComponent(GameObject* owner, float width, float height, float offsetX, float offsetY)
        : Component(owner)
        , m_width(width)
        , m_height(height)
        , m_offsetX(offsetX)
        , m_offsetY(offsetY)
    {
        s_allHitboxes.push_back(this);
    }

    HitboxComponent::~HitboxComponent()
    {
        s_allHitboxes.erase(
            std::remove(s_allHitboxes.begin(), s_allHitboxes.end(), this),
            s_allHitboxes.end()
        );
    }

    bool HitboxComponent::Overlaps(const HitboxComponent* other) const
    {
        auto* myTransform = GetOwner()->GetComponent<TransformComponent>();
        auto* otherTransform = other->GetOwner()->GetComponent<TransformComponent>();
        if (!myTransform || !otherTransform) return false;

        const auto& myPos = myTransform->GetWorldPosition();
        const auto& otherPos = otherTransform->GetWorldPosition();

        const float myLeft = myPos.x + m_offsetX;
        const float myTop = myPos.y + m_offsetY;
        const float myRight = myLeft + m_width;
        const float myBottom = myTop + m_height;

        const float otherLeft = otherPos.x + other->m_offsetX;
        const float otherTop = otherPos.y + other->m_offsetY;
        const float otherRight = otherLeft + other->m_width;
        const float otherBottom = otherTop + other->m_height;

        return myLeft < otherRight &&
            myRight > otherLeft &&
            myTop   < otherBottom &&
            myBottom > otherTop;
    }

    void HitboxComponent::Update()
    {
        if (GetOwner()->m_destroy) return;

        for (auto* other : s_allHitboxes)
        {
            if (other == this) continue;
            if (other->GetOwner()->m_destroy) continue;
            if (!Overlaps(other)) continue;

            EventManager::GetInstance().SendEvent(
                std::make_unique<DataEvent<OverlapEvent>>(
                    ACTOR_OVERLAPPED,
                    OverlapEvent{ GetOwner(), other->GetOwner() }
                )
            );
        }
    }
}