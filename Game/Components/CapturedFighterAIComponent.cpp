#include "CapturedFighterAIComponent.h"
#include "ShootComponent.h"
#include "GameEvents.h"
#include "GameObject.h"
#include "Components/TransformComponent.h"
#include "Components/RenderComponent.h"
#include "TimeManager.h"
#include <algorithm>
#include <cmath>

dae::CapturedFighterAIComponent::CapturedFighterAIComponent(GameObject* owner, GameObject* boss, const glm::vec3& startPos)
    : Component(owner)
    , m_boss(boss)
    , m_startPos(startPos)
{
    m_shoot = owner->GetComponent<ShootComponent>();
    m_transform = owner->GetComponent<TransformComponent>();

    m_bossTransform = m_boss->GetComponent<TransformComponent>();
    m_bossRender = m_boss->GetComponent<RenderComponent>();
    m_selfRender = GetOwner()->GetComponent<RenderComponent>();

    EventManager::GetInstance().AttachEvent(DATEVENT_ENEMY_SHOT, this);
}

glm::vec2 dae::CapturedFighterAIComponent::GetDockPosition() const
{
    const auto& bossPos = m_bossTransform->GetWorldPosition();
    const auto& bossScale = m_bossTransform->GetScale();
    const auto& selfScale = m_transform->GetScale();
    const auto bossFrameSize = m_bossRender->GetSize();
    const auto selfSize = m_selfRender->GetSize();

    const float bossVisualLeft = bossPos.x - bossFrameSize.x * (bossScale.x - 1.f) * 0.5f;
    const float bossVisualTop = bossPos.y - bossFrameSize.y * (bossScale.y - 1.f) * 0.5f;
    const float bossVisualWidth = bossFrameSize.x * bossScale.x;

    const float x = bossVisualLeft + (bossVisualWidth - selfSize.x) * 0.5f;
    const float y = bossVisualTop - selfSize.y * (selfScale.y + 1.f) * 0.5f;

    return { x, y };
}

void dae::CapturedFighterAIComponent::Update()
{
    if (!m_bossTransform || !m_bossRender || !m_selfRender) return;

    const auto dock = GetDockPosition();

    if (m_capturing)
    {
        m_animTimer += TimeManager::GetInstance().GetDeltaTime();
        const float t = std::clamp(m_animTimer / k_animDuration, 0.f, 1.f);

        const float x = m_startPos.x + (dock.x - m_startPos.x) * t;
        const float y = m_startPos.y + (dock.y - m_startPos.y) * t;
        m_transform->SetLocalPosition({ x, y, 0.f });
        m_transform->SetRotation(0.f, 0.f, std::fmod(k_totalSpinDegrees * t, 360.f));

        if (t >= 1.f)
            m_capturing = false;

        return;
    }

    m_transform->SetLocalPosition({ dock.x, dock.y, 0.f });
}

void dae::CapturedFighterAIComponent::HandleEvent(const Event* pEvent)
{
    if (pEvent->id != DATEVENT_ENEMY_SHOT) return;

    const auto* e = static_cast<const DataEvent<ActorEvent>*>(pEvent);
    if (e->data.actor != m_boss) return;

    if (m_shoot) m_shoot->Shoot(0.f, 1.f);
}
