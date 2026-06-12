#pragma once
#include "Commands/Command.h"
#include "Components/PlayerHealthComponent.h"
#include "GameObject.h"


class KillCommand : public GameObjectCommand
{
    dae::PlayerHealthComponent* healthComp;
public:
    KillCommand(dae::GameObject* gameObject)
        : GameObjectCommand(gameObject)
    {
        healthComp = GetGameObject()->GetComponent<dae::PlayerHealthComponent>();
    }

    void Execute() override
    {
        healthComp->TakeDamage(1.f);
    }
};