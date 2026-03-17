#pragma once
#include "Command.h"
#include "../Components/HealthComponent.h"
#include "../GameObject.h"


class KillCommand : public GameObjectCommand
{
    dae::HealthComponent* healthComp;
public:
    KillCommand(dae::GameObject* gameObject)
        : GameObjectCommand(gameObject)
    {
        healthComp = GetGameObject()->GetComponent<dae::HealthComponent>();
    }

    void Execute() override
    {
        healthComp->TakeDamage(1.f);
    }
};