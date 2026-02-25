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
	if (m_enemyTransforms.size() < m_allEnemies)
		return;

	Breathe();
	LeftRight();
}

void dae::FormationComponent::LeftRight()
{
	m_time += TimeManager::GetInstance().GetDeltaTime();
	if (m_time >= 0.45f)
	{
		m_time = 0.f;
		if (m_moveLeft) m_horizontalOffset *= -1.f;
		m_transform->SetLocalPosition({ m_transform->GetLocalPosition().x + m_horizontalOffset, m_transform->GetLocalPosition().y, 0.f });
		m_moveLeft = !m_moveLeft;
	}
}

void dae::FormationComponent::RegisterEnemy(TransformComponent* enemyTransform)
{
	m_enemyTransforms.push_back(enemyTransform);
	m_originalLocalPositions.push_back(enemyTransform->GetLocalPosition());

	if (m_originalLocalPositions.empty())
		return;

	glm::vec3 sum{};
	float minY = std::numeric_limits<float>::max();


	for (const auto& pos : m_originalLocalPositions)
	{
		sum.x += pos.x;

		if (pos.y < minY)
			minY = pos.y;
	}

	m_center.x = sum.x / static_cast<float>(m_originalLocalPositions.size());
	m_center.y = minY;
	m_center.z = 0.f;
}

void dae::FormationComponent::Breathe()
{
	m_breathTime += TimeManager::GetInstance().GetDeltaTime();

	float angle = fmod(m_breathTime * m_breathSpeed, 2.0f * 3.14159265f);
	float offsetAmount = sinf(angle) * m_breathAmount;

	for (size_t i = 0; i < m_enemyTransforms.size(); ++i)
	{
		glm::vec3 original = m_originalLocalPositions[i];

		// Horizontal offset
		float distanceX = original.x - m_center.x;
		float horizontalOffset = 0.f;
		if (std::abs(distanceX) > 0.001f)
		{
			horizontalOffset = std::abs(offsetAmount) * (std::abs(distanceX) * 0.1f);
			horizontalOffset *= (distanceX > 0.f) ? 1.f : -1.f;
		}

		// Vertical offset
		float distanceY = original.y - m_center.y;
		float verticalOffset = 0.f;
		if (std::abs(distanceY) > 0.001f)
		{
			verticalOffset = std::abs(offsetAmount) * (std::abs(distanceY) * 0.1f);
			verticalOffset *= (distanceY > 0.f) ? 1.f : -1.f;
		}

		// Apply both offsets together
		m_enemyTransforms[i]->SetLocalPosition({
			original.x + horizontalOffset,
			original.y + verticalOffset,
			original.z
			});
	}
}

void dae::FormationComponent::SetAllEnemies(int enemyCount)
{ 
	m_allEnemies = enemyCount;
}