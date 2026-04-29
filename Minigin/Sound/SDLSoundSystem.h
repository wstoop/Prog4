#pragma once
#include "SoundSystem.h"
#include <memory>
#include <string>

namespace dae
{
    class SDLSoundSystem : public SoundSystem
    {
    public:
        SDLSoundSystem();
        ~SDLSoundSystem();

        void Play(sound_id id, float volume) override;
        void RegisterSound(sound_id id, const std::string& path) override;

    private:
        struct Impl;
        std::unique_ptr<Impl> m_pImpl;
    };
}
