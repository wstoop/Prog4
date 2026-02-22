#include "FormationComponent.h"
#include "TransformComponent.h"
#include "../GameObject.h"
#include "../TimeManager.h"

dae::FormationComponent::FormationComponent(GameObject* owner)
    : Component(owner)
{
	m_transform = GetOwner()->GetComponent<TransformComponent>();
	m_allEnemies = 999;
}

void dae::FormationComponent::Update()
{
	LeftRight();
}

void dae::FormationComponent::LeftRight()
{
	if(m_dockedEnemies < m_allEnemies)
		return;

	m_time += TimeManager::GetInstance().GetDeltaTime();
	if (m_time >= 0.45f)
	{
		m_time = 0.f;
		if (m_moveLeft) m_horizontalOffset *= -1.f;
		m_transform->SetLocalPosition({ m_transform->GetLocalPosition().x + m_horizontalOffset, m_transform->GetLocalPosition().y, 0.f });
		m_moveLeft = !m_moveLeft;
	}
}