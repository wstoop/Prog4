#pragma once
#include "Commands/Command.h"
#include "GameObject.h"
#include "Components/ShootComponent.h"
#include "Components/PlayerHealthComponent.h"
#include "TimeManager.h"

class ShootCommand : public GameObjectCommand
{

    dae::ShootComponent* m_shootComponent;
	dae::PlayerHealthComponent* m_health;
	dae::GameObject* m_target;

public:
    ShootCommand(dae::GameObject* gameObject)
        : GameObjectCommand(gameObject)
    {
        m_shootComponent = gameObject->GetComponent<dae::ShootComponent>();
        m_health = gameObject->GetComponent<dae::PlayerHealthComponent>();
        m_target = gameObject;

    }

    void Execute() override
    {
        if (GetGameObject()->m_destroy) return;
        if (m_health && m_health->IsDead()) return;
		m_shootComponent->Shoot(0.f, -1.f);
    }
};