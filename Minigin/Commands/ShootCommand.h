#pragma once
#include "Command.h"
#include "../GameObject.h"
#include "../Components/ShootComponent.h"

class ShootCommand : public GameObjectCommand
{
    
    dae::ShootComponent* m_shootComponent;
public:
    ShootCommand(dae::GameObject* gameObject)
        : GameObjectCommand(gameObject)
    {
        m_shootComponent = gameObject->GetComponent<dae::ShootComponent>();

    }

    void Execute() override
    {
		m_shootComponent->Shoot(0.f, -1.f);
    }
};