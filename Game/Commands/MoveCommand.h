#pragma once
#include "Commands/Command.h"
#include "GameObject.h"
#include "Components/TransformComponent.h"
#include "Components/TextureComponent.h"
#include "Components/PlayerHealthComponent.h"
#include "TimeManager.h"
#include "GameInfo.h"
#include <glm/glm.hpp>
#include <algorithm>
class MoveCommand : public GameObjectAxisCommand
{
    float m_speed;
	dae::TransformComponent* m_transform;
	dae::PlayerHealthComponent* m_health;
	glm::vec2 m_frameSize;
public:
    MoveCommand(dae::GameObject* gameObject, float speed)
        : GameObjectAxisCommand(gameObject), m_speed(speed)
    {
		m_transform = gameObject->GetComponent<dae::TransformComponent>();
		m_health = gameObject->GetComponent<dae::PlayerHealthComponent>();
		m_frameSize = gameObject->GetComponent<dae::TextureComponent>()->GetSize();
    }

    void Execute(glm::vec2 axis) override
    {
        if (GetGameObject()->m_destroy) return;
        if (m_health && m_health->IsDead()) return;
        auto d = dae::TimeManager::GetInstance().GetDeltaTime();
        auto pos = m_transform->GetLocalPosition();
        pos.x += axis.x * m_speed * d;

        const auto& scale = m_transform->GetScale();
        const float minX = m_frameSize.x * (scale.x - 1.f) * 0.5f;
        const float maxX = static_cast<float>(dae::GameInfo::GetInstance().GetGameWidth()) - m_frameSize.x * (scale.x + 1.f) * 0.5f;
        pos.x = std::clamp(pos.x, minX, maxX);

        m_transform->SetLocalPosition(pos);

    }
};