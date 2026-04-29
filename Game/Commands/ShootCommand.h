#pragma once
#include "Commands/Command.h"
#include "GameObject.h"
#include "Components/ShootComponent.h"

class ShootCommand : public GameObjectCommand
{
    
    dae::ShootComponent* m_shootComponent;
	dae::GameObject* m_target;
public:
    ShootCommand(dae::GameObject* gameObject)
        : GameObjectCommand(gameObject)
    {
        m_shootComponent = gameObject->GetComponent<dae::ShootComponent>();
        m_target = gameObject;
    }

    void Execute() override
    {
        if (GetGameObject()->m_destroy) return;
		m_shootComponent->Shoot(0.f, -1.f);
    }
};