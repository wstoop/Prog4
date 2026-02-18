#include "OrbitComponent.h"
#include "TransformComponent.h"
#include "../GameObject.h"
#include "../TimeManager.h"

dae::OrbitComponent::OrbitComponent(float radius, float speed, float startAngle)
    : m_radius(radius)
    , m_speed(speed)
    , m_angle(startAngle)
{
}

void dae::OrbitComponent::Update()
{
    auto transform = GetOwner()->GetComponent<TransformComponent>();
    if (!transform)
        return;

    if (!transform->GetParent())
        return;

    m_angle += m_speed * TimeManager::GetInstance().GetDeltaTime();

    float x = std::cos(m_angle) * m_radius;
    float y = std::sin(m_angle) * m_radius;

    transform->SetLocalPosition(glm::vec3(x, y, 0.f));
}