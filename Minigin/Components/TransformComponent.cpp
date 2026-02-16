#include "TransformComponent.h"

void dae::TransformComponent::SetPosition(const float x, const float y, const float z)
{
	m_pos.x = x;
	m_pos.y = y;
	m_pos.z = z;
}
void dae::TransformComponent::SetPosition(const glm::vec3& position)
{
	m_pos = position;
}

void dae::TransformComponent::SetScale(const float x, const float y, const float z)
{
	m_scale.x = x;
	m_scale.y = y;
	m_scale.z = z;
}
void dae::TransformComponent::SetScale(const glm::vec3& scale)
{
	m_scale = scale;
}

void dae::TransformComponent::SetRotation(const float x, const float y, const float z)
{
	m_rotation.x = x;
	m_rotation.y = y;
	m_rotation.z = z;
}
void dae::TransformComponent::SetRotation(const glm::vec3& rotation)
{
	m_rotation = rotation;
}