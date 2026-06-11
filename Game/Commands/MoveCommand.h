#pragma once
#include "Commands/Command.h"
#include "GameObject.h"
#include "Components/TransformComponent.h"
#include "TimeManager.h"
#include <glm/glm.hpp>
class MoveCommand : public GameObjectAxisCommand
{
    float m_speed;
	dae::TransformComponent* m_transform;
public:
    MoveCommand(dae::GameObject* gameObject, float speed)
        : GameObjectAxisCommand(gameObject), m_speed(speed)
    {
		m_transform = gameObject->GetComponent<dae::TransformComponent>();
    }

    void Execute(glm::vec2 axis) override
    {
        if (GetGameObject()->m_destroy) return;
        auto d = dae::TimeManager::GetInstance().GetDeltaTime();
        auto pos = m_transform->GetLocalPosition();
        pos.x += axis.x * m_speed * d;
        m_transform->SetLocalPosition(pos);

    }
};