#include "SDLSoundSystem.h"
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <unordered_map>
#include <atomic>
#include <iostream>

namespace dae
{
    struct SoundRequest
    {
        sound_id id;
        float    volume;
    };

    struct SDLSoundSystem::Impl
    {
        MIX_Mixer* m_Mixer{ nullptr };

        std::unordered_map<sound_id, std::string> m_Paths;
        std::unordered_map<sound_id, MIX_Audio*>  m_Audio;

        std::queue<SoundRequest> m_Queue;
        std::mutex               m_Mutex;
        std::condition_variable  m_CV;

        std::thread m_Thread;
        bool        m_Running{ true };
        std::atomic<bool> m_Muted{ false };

        Impl()
        {
            if (!MIX_Init())
            {
                std::cerr << "MIX_Init failed: " << SDL_GetError() << "\n";
                return;
            }

            m_Mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
            if (!m_Mixer)
            {
                std::cerr << "MIX_CreateMixerDevice failed: " << SDL_GetError() << "\n";
                MIX_Quit();
                return;
            }

            m_Thread = std::thread(&Impl::ProcessLoop, this);
        }

        ~Impl()
        {
            {
                std::lock_guard lock(m_Mutex);
                m_Running = false;
            }
            m_CV.notify_all();

            if (m_Thread.joinable())
                m_Thread.join();

        }

        void Enqueue(sound_id id, float volume)
        {
            if (m_Muted)
                return;

            std::lock_guard lock(m_Mutex);
            if (!m_Running)
                return;
            m_Queue.push({ id, volume });

            m_CV.notify_one();
        }

        void ProcessLoop()
        {
            if (!m_Mixer)
                return;

            while (true)
            {
                SoundRequest req;

                // --- WAIT FOR WORK ---
                {
                    std::unique_lock lock(m_Mutex);
                    m_CV.wait(lock, [this] { return !m_Queue.empty() || !m_Running; });

                    if (!m_Running && m_Queue.empty())
                        break;

                    req = m_Queue.front();
                    m_Queue.pop();
                }

                // --- LOAD AUDIO SAFELY ---
                MIX_Audio* audio = nullptr;
                std::string path;

                {
                    std::lock_guard lock(m_Mutex);

                    // Already loaded?
                    auto itAudio = m_Audio.find(req.id);
                    if (itAudio != m_Audio.end())
                    {
                        audio = itAudio->second;
                    }
                    else
                    {
                        auto itPath = m_Paths.find(req.id);
                        if (itPath == m_Paths.end())
                        {
                            std::cerr << "No path registered for sound id " << req.id << "\n";
                            continue;
                        }

                        path = itPath->second; // copy string safely
                    }
                }

                // Load audio outside lock
                if (!audio)
                {
                    audio = MIX_LoadAudio(m_Mixer, path.c_str(), true);
                    if (!audio)
                    {
                        std::cerr << "MIX_LoadAudio failed for " << path
                            << ": " << SDL_GetError() << "\n";
                        continue;
                    }

                    // Store loaded audio
                    {
                        std::lock_guard lock(m_Mutex);
                        m_Audio[req.id] = audio;
                    }
                }

                // --- PLAY SOUND ---
                MIX_Track* track = MIX_CreateTrack(m_Mixer);
                if (!track)
                {
                    std::cerr << "MIX_CreateTrack failed: " << SDL_GetError() << "\n";
                    continue;
                }

                MIX_SetTrackAudio(track, audio);
                MIX_SetTrackGain(track, req.volume);
                MIX_PlayTrack(track, 0);
            }
        }
    };

    SDLSoundSystem::SDLSoundSystem()
        : m_pImpl(std::make_unique<Impl>())
    {
    }

    SDLSoundSystem::~SDLSoundSystem() = default;

    void SDLSoundSystem::Play(sound_id id, float volume)
    {
        if (!m_pImpl) return;
        m_pImpl->Enqueue(id, volume);
    }

    void SDLSoundSystem::SetMuted(bool muted)
    {
        if (m_pImpl) m_pImpl->m_Muted = muted;
    }

    bool SDLSoundSystem::IsMuted() const
    {
        return m_pImpl && m_pImpl->m_Muted;
    }

    void SDLSoundSystem::RegisterSound(sound_id id, const std::string& path)
    {
        std::lock_guard lock(m_pImpl->m_Mutex);

        if (path.starts_with("./Data/") || path.starts_with("../Data/"))
        {
            m_pImpl->m_Paths[id] = path;
        }
        else
        {
            m_pImpl->m_Paths[id] = "./Data/" + path;
        }
    }
}