#pragma once
#include "SoundSystem.h"
#include <iostream>
#include <memory>

namespace dae
{
    class LoggingSoundSystem final : public SoundSystem
    {
        std::unique_ptr<SoundSystem> m_RealSS;

    public:
        explicit LoggingSoundSystem(std::unique_ptr<SoundSystem> ss)
            : m_RealSS(std::move(ss)) {
        }

        void Play(sound_id id, float volume) override
        {
            m_RealSS->Play(id, volume);
            std::cout << "[Sound] Play id=" << id << " vol=" << volume << "\n";
        }

        void RegisterSound(sound_id id, const std::string& path) override
        {
            m_RealSS->RegisterSound(id, path);
            std::cout << "[Sound] Register id=" << id << " -> " << path << "\n";
        }
    };
}