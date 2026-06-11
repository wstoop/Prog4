#include "FormationComponent.h"
#include "Components/TransformComponent.h"
#include "GameObject.h"
#include "TimeManager.h"
#include "EventManager.h"
#include "GameEvents.h"
#include <cmath>

dae::FormationComponent::FormationComponent(GameObject* owner)
    : Component(owner)
{
    m_transform = GetOwner()->GetComponent<TransformComponent>();
    m_allEnemies = 999;

    EventManager::GetInstance().AttachEvent(EVENT_PLAYER_TOOK_DAMAGE, this);
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

    if (m_waitingForRegroup)
    {
        if (m_inFormationCount >= m_aliveEnemyCount)
        {
            m_waitingForRegroup = false;
            EventManager::GetInstance().SendEvent(EVENT_ALL_ENEMIES_RETURNED);
        }
    }
}

void dae::FormationComponent::LeftRight()
{
    m_time += dae::TimeManager::GetInstance().GetDeltaTime();
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
    m_breathTime += dae::TimeManager::GetInstance().GetDeltaTime();
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
    m_aliveEnemyCount = enemyCount;
}

void dae::FormationComponent::NotifyDied()
{
    if (m_inFormationCount > 0)
        --m_inFormationCount;
    --m_aliveEnemyCount;

    if (!m_started)
        ++m_dockedCount;

    if (m_aliveEnemyCount <= 0)
        EventManager::GetInstance().SendEvent(EVENT_WAVE_CLEARED);
}


void dae::FormationComponent::NotifyDocked()
{
    ++m_dockedCount;       // gates m_started — never decrements
    ++m_inFormationCount;  // current occupancy — can go down during dives
}

void dae::FormationComponent::NotifyUndocked()
{
    // Don't touch m_dockedCount — the start gate must never revert.
    if (m_inFormationCount > 0)
        --m_inFormationCount;
}

void dae::FormationComponent::HandleEvent(const Event* pEvent)
{
    switch (pEvent->id)
    {
    case EVENT_PLAYER_TOOK_DAMAGE:
		m_waitingForRegroup = true;
        break;
    }
}