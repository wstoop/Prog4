#include "FormationComponent.h"
#include "TransformComponent.h"
#include "../GameObject.h"
#include "../TimeManager.h"
#include <cmath>

dae::FormationComponent::FormationComponent(GameObject* owner)
    : Component(owner)
{
    m_transform = GetOwner()->GetComponent<TransformComponent>();
    m_allEnemies = 999;
}

void dae::FormationComponent::Update()
{
    if (!m_started)
    {
        if (m_dockedCount < m_allEnemies)
            return;
        m_started = true;
    }

    AdvanceBreath();
    LeftRight();
}

void dae::FormationComponent::LeftRight()
{
    m_time += TimeManager::GetInstance().GetDeltaTime();
    if (m_time >= 0.45f)
    {
        m_time = 0.f;
        if (m_moveLeft) m_horizontalOffset *= -1.f;
        m_transform->SetLocalPosition({
            m_transform->GetLocalPosition().x + m_horizontalOffset,
            m_transform->GetLocalPosition().y,
            0.f });
        m_moveLeft = !m_moveLeft;
    }
}

void dae::FormationComponent::AdvanceBreath()
{
    m_breathTime += TimeManager::GetInstance().GetDeltaTime();
}

void dae::FormationComponent::NotifyDocked()
{
    ++m_dockedCount;
}

glm::vec3 dae::FormationComponent::ComputeSwayOffset(const glm::vec3& slotLocalPos, const glm::vec3& center) const
{
    float angle = fmod(m_breathTime * m_breathSpeed, 2.0f * 3.14159265f);
    float offsetAmount = sinf(angle) * m_breathAmount;

    // Horizontal offset
    float distanceX = slotLocalPos.x - center.x;
    float horizontalOffset = 0.f;
    if (std::abs(distanceX) > 0.001f)
    {
        horizontalOffset = std::abs(offsetAmount) * (std::abs(distanceX) * 0.1f);
        horizontalOffset *= (distanceX > 0.f) ? 1.f : -1.f;
    }

    // Vertical offset
    float distanceY = slotLocalPos.y - center.y;
    float verticalOffset = 0.f;
    if (std::abs(distanceY) > 0.001f)
    {
        verticalOffset = std::abs(offsetAmount) * (std::abs(distanceY) * 0.1f);
        verticalOffset *= (distanceY > 0.f) ? 1.f : -1.f;
    }

    return { horizontalOffset, verticalOffset, 0.f };
}

void dae::FormationComponent::SetAllEnemies(int enemyCount)
{
    m_allEnemies = enemyCount;
}

void dae::FormationComponent::NotifyDied()
{
    ++m_dockedCount;
}