#pragma once
#include "Components/Component.h"
#include "EventManager.h"
#include <string>
#include <vector>
#include <memory>

namespace dae
{
    class Scene;
    class GameObject;

    struct WaveData
    {
        std::vector<std::string> grid;

        float spacingX{ 45.f };
        float spacingY{ 40.f };
        float entryDuration{ 2.0f };
        float entryDelayPerRow{ 0.05f };

        float formationX{ 120.f };
        float formationY{ 200.f };
    };

    struct PrebuiltWave
    {
        std::unique_ptr<GameObject> formationMover;
        std::vector<std::unique_ptr<GameObject>> enemies;
    };


    class WaveManager final : public Component, public EventHandler
    {
    public:
        WaveManager(GameObject* owner, Scene& scene, const std::string& waveFile);
        ~WaveManager() override = default;

        void HandleEvent(const Event* pEvent) override;

        void StartFirstWave();

        int  GetCurrentWaveIndex() const { return m_currentWaveIndex; }
        bool AllWavesCleared()     const { return m_allCleared; }

    private:
        void SpawnWave(int index);
        void DestroyCurrentFormation();

        static std::vector<WaveData> LoadWavesFromFile(const std::string& waveFile);
        void PreloadAllWaves();

        Scene& m_scene;
        std::vector<WaveData> m_waves;
        std::vector<PrebuiltWave> m_prebuiltWaves;

        int m_currentWaveIndex{ -1 };
        bool m_allCleared{ false };
        GameObject* m_pCurrentFormation{ nullptr };
    };
}