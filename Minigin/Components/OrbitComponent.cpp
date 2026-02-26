#include "OrbitComponent.h"
#include "TransformComponent.h"
#include "../GameObject.h"
#include "../TimeManager.h"

dae::OrbitComponent::OrbitComponent(GameObject* owner, float radius, float speed, float startAngle)
    : Component(owner)
    , m_radius(radius)
    , m_speed(speed)
    , m_angle(startAngle)
{
	m_transform = GetOwner()->GetComponent<TransformComponent>();
}

void dae::OrbitComponent::Update()
{
    if (!m_transform)
        return;
    m_angle += m_speed * TimeManager::GetInstance().GetDeltaTime();
	if (m_angle > 2.f * 3.14159265f)
        m_angle = fmod(m_angle, 2.f * 3.14159265f);
	if (m_angle < -2.f * 3.14159265f)
        m_angle = fmod(m_angle, -2.f * 3.14159265f);

    float x = std::cos(m_angle) * m_radius;
    float y = std::sin(m_angle) * m_radius;

    m_transform->SetLocalPosition(glm::vec3(x, y, 0.f));
}