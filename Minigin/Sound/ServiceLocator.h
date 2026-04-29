#pragma once
#include <memory>
#include "SoundSystem.h"

namespace dae
{
    class ServiceLocator final
    {
        static std::unique_ptr<SoundSystem> m_SoundInstance;
    public:
        static SoundSystem& GetSoundSystem() { return *m_SoundInstance; }

        static void RegisterSoundSystem(std::unique_ptr<SoundSystem>&& ss)
        {
            m_SoundInstance = ss == nullptr
                ? std::make_unique<NullSoundSystem>()
                : std::move(ss);
        }
    };
}