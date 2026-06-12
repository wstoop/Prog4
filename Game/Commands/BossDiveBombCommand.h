#pragma once
#include "Commands/Command.h"
#include "Components/BossPlayerControlComponent.h"
#include "GameObject.h"

class BossDiveBombCommand : public GameObjectCommand
{
public:
    using GameObjectCommand::GameObjectCommand;

    void Execute() override
    {
        if (GetGameObject()->m_destroy) return;
        GetGameObject()->GetComponent<dae::BossPlayerControlComponent>()->TriggerDiveBomb();
    }
};
