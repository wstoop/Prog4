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

        void SetMuted(bool muted) override
        {
            m_RealSS->SetMuted(muted);
            std::cout << "[Sound] Muted=" << muted << "\n";
        }

        bool IsMuted() const override { return m_RealSS->IsMuted(); }
    };
}