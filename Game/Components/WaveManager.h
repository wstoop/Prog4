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

    // Describes one wave loaded from JSON
    struct WaveData
    {
        // 2-D grid of enemy chars, same convention as the old .txt files
        // '.' = empty, 'B' = bee, 'W' = butterfly, 'G' = boss
        std::vector<std::string> grid;

        float spacingX{ 45.f };
        float spacingY{ 40.f };
        float entryDuration{ 2.0f };
        float entryDelayPerRow{ 0.05f };

        // Formation mover world-space origin
        float formationX{ 120.f };
        float formationY{ 200.f };
    };

    // A fully wired wave — formation mover + all enemies — ready to move into
    // the scene in one go. Built during PreloadAllWaves, consumed by SpawnWave.
    struct PrebuiltWave
    {
        std::unique_ptr<GameObject> formationMover;
        std::vector<std::unique_ptr<GameObject>> enemies;
    };

    // Component that lives on a dedicated GameObject in the game scene.
    // It listens to EVENT_WAVE_CLEARED and spawns the next wave.
    // All enemy GameObjects AND their formation mover are pre-built at
    // construction time. SpawnWave only moves them into the scene.
    //
    // Usage:
    //   auto waveManagerGO = std::make_unique<dae::GameObject>();
    //   waveManagerGO->AddComponent<dae::WaveManager>(scene, "waves.json");
    //   scene.Add(std::move(waveManagerGO));
    class WaveManager final : public Component, public EventHandler
    {
    public:
        WaveManager(GameObject* owner, Scene& scene, const std::string& waveFile);
        ~WaveManager() override = default;

        // EventHandler — reacts to EVENT_WAVE_CLEARED
        void HandleEvent(const Event* pEvent) override;

        // Kick off wave 0 manually (call after the scene is fully built)
        void StartFirstWave();

        int  GetCurrentWaveIndex() const { return m_currentWaveIndex; }
        bool AllWavesCleared()     const { return m_allCleared; }

    private:
        void SpawnWave(int index);
        void DestroyCurrentFormation();

        static std::vector<WaveData> LoadWavesFromFile(const std::string& waveFile);
        void PreloadAllWaves();

        Scene& m_scene;
        std::vector<WaveData>        m_waves;
        std::vector<PrebuiltWave>    m_prebuiltWaves;

        int          m_currentWaveIndex{ -1 };
        bool         m_allCleared{ false };
        GameObject* m_pCurrentFormation{ nullptr };
    };
}