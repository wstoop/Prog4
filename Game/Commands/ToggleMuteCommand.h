#pragma once
#include "Commands/Command.h"
#include "Sound/ServiceLocator.h"

class ToggleMuteCommand : public Command
{
public:
    void Execute() override
    {
        auto& sound = dae::ServiceLocator::GetSoundSystem();
        sound.SetMuted(!sound.IsMuted());
    }
};
