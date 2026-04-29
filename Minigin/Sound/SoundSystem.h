#pragma once
#include <string>

namespace dae
{
    using sound_id = unsigned short;

    class SoundSystem
    {
    public:
        virtual ~SoundSystem() = default;
        virtual void Play(sound_id id, float volume) = 0;
        virtual void RegisterSound(sound_id id, const std::string& path) = 0;
    };

    class NullSoundSystem final : public SoundSystem
    {
    public:
        void Play(sound_id, float) override {}
        void RegisterSound(sound_id, const std::string&) override {}
    };
}