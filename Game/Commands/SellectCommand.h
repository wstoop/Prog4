#pragma once
#include "Command.h"
#include "../GameObject.h"

class SellectCommand : public GameObjectCommand
{
public:
    SellectCommand(dae::GameObject* gameObject)
        : GameObjectCommand(gameObject)
    {

    }
      
    void Execute() override
    {

    }
};