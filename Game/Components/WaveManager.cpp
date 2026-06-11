#include "WaveManager.h"

#include "Scene.h"
#include "GameObject.h"
#include "ResourceManager.h"
#include "EnemyFactory.h"
#include "States/Enemies/EnemyBrainComponent.h"
#include "GameEvents.h"

#include "Components/TransformComponent.h"
#include "Components/FormationComponent.h"
#include "Components/EnemyFormationSlotComponent.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <stdexcept>
#include <limits>
#include <algorithm>

using json = nlohmann::json;

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

dae::WaveManager::WaveManager(GameObject* owner, Scene& scene, const std::string& waveFile)
    : Component(owner)
    , m_scene(scene)
    , m_waves(LoadWavesFromFile(waveFile))
{
    EventManager::GetInstance().AttachEvent(EVENT_WAVE_CLEARED, this);
    EnemyFactory::RegisterDefaults();
    PreloadAllWaves();
}

// ---------------------------------------------------------------------------
// EventHandler
// ---------------------------------------------------------------------------

void dae::WaveManager::HandleEvent(const Event* /*pEvent*/)
{
    DestroyCurrentFormation();

    const int next = m_currentWaveIndex + 1;
    if (next >= static_cast<int>(m_prebuiltWaves.size()))
    {
        m_allCleared = true;
        EventManager::GetInstance().SendEvent(EVENT_LOAD_END);
        return;
    }

    SpawnWave(next);
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void dae::WaveManager::StartFirstWave()
{
    if (m_prebuiltWaves.empty()) return;
    SpawnWave(0);
}

// ---------------------------------------------------------------------------
// Private — pre-loading
// ---------------------------------------------------------------------------

void dae::WaveManager::PreloadAllWaves()
{
    m_prebuiltWaves.resize(m_waves.size());

    for (size_t wi = 0; wi < m_waves.size(); ++wi)
    {
        const WaveData& wave = m_waves[wi];
        PrebuiltWave& built = m_prebuiltWaves[wi];

        // --- Formation mover ---
        built.formationMover = std::make_unique<dae::GameObject>();
        built.formationMover->GetComponent<dae::TransformComponent>()
            ->SetLocalPosition({ wave.formationX, wave.formationY, 0.f });
        built.formationMover->AddComponent<dae::FormationComponent>();

        auto* formationPtr = built.formationMover.get();
        auto* formationComp = formationPtr->GetComponent<dae::FormationComponent>();

        // --- Compute formation center for sway ---
        glm::vec3 formationCenter{};
        {
            glm::vec3 sum{};
            float minY = std::numeric_limits<float>::max();
            int   count = 0;
            for (size_t row = 0; row < wave.grid.size(); ++row)
                for (size_t col = 0; col < wave.grid[row].size(); ++col)
                {
                    if (wave.grid[row][col] == '.') continue;
                    sum.x += static_cast<float>(col) * wave.spacingX;
                    float y = static_cast<float>(row) * wave.spacingY;
                    if (y < minY) minY = y;
                    ++count;
                }
            if (count > 0)
            {
                formationCenter.x = sum.x / static_cast<float>(count);
                formationCenter.y = minY;
            }
        }

        // --- Compute per-column entry direction ---
        const int totalCols = static_cast<int>(wave.grid[0].size());
        std::vector<bool> colFromLeft(totalCols, true);
        {
            bool fromLeft = false;
            for (int i = 0; i < totalCols / 2; ++i)
            {
                colFromLeft[i] = fromLeft;
                colFromLeft[totalCols - 1 - i] = !fromLeft;
                fromLeft = !fromLeft;
            }
        }

        // --- Spawn enemies ---
        int enemyCount = 0;
        for (size_t row = 0; row < wave.grid.size(); ++row)
        {
            for (size_t col = 0; col < wave.grid[row].size(); ++col)
            {
                char cell = wave.grid[row][col];
                if (cell == '.') continue;

                auto enemy = dae::EnemyFactory::Create(cell);
                if (!enemy) continue;

                enemy->tag = "Enemy";
                enemy->GetComponent<dae::TransformComponent>()
                    ->SetScale({ 3.f, 3.f, 0.f });

                glm::vec3 localTarget{
                    static_cast<float>(col) * wave.spacingX,
                    static_cast<float>(row) * wave.spacingY,
                    0.f
                };
                glm::vec3 worldTarget{
                    formationPtr->GetComponent<dae::TransformComponent>()->GetLocalPosition().x + localTarget.x,
                    formationPtr->GetComponent<dae::TransformComponent>()->GetLocalPosition().y + localTarget.y,
                    0.f
                };

                const float delay = wave.entryDelayPerRow * static_cast<float>(row);
                const bool  fromLeft = colFromLeft[col];

                enemy->AddComponent<dae::EnemyFormationSlotComponent>(
                    formationComp, localTarget, formationCenter);

                auto* brain = enemy->AddComponent<dae::EnemyBrainComponent>();
                brain->SetEntryConfig(worldTarget, wave.entryDuration, delay, fromLeft, formationPtr);

                built.enemies.push_back(std::move(enemy));
                ++enemyCount;
            }
        }

        formationComp->SetAllEnemies(enemyCount);
    }
}

// ---------------------------------------------------------------------------
// Private — spawning
// ---------------------------------------------------------------------------

void dae::WaveManager::SpawnWave(int index)
{
    m_currentWaveIndex = index;
    PrebuiltWave& built = m_prebuiltWaves[index];

    m_scene.Reserve(built.enemies.size() + 1);

    for (auto& enemy : built.enemies)
        m_scene.Add(std::move(enemy));

    m_pCurrentFormation = built.formationMover.get();
    m_scene.Add(std::move(built.formationMover));
}

void dae::WaveManager::DestroyCurrentFormation()
{
    if (!m_pCurrentFormation) return;
    m_pCurrentFormation->m_destroy = true;
    m_pCurrentFormation = nullptr;
}

// ---------------------------------------------------------------------------
// JSON loading
// ---------------------------------------------------------------------------

std::vector<dae::WaveData> dae::WaveManager::LoadWavesFromFile(const std::string& waveFile)
{
    const std::string fullPath =
        dae::ResourceManager::GetInstance().GetDataPath().string() + waveFile;

    std::ifstream file(fullPath);
    if (!file.is_open())
        throw std::runtime_error("WaveManager: could not open wave file: " + fullPath);

    json root;
    file >> root;

    std::vector<WaveData> waves;
    for (const auto& waveJson : root.at("waves"))
    {
        WaveData wd;

        for (const auto& row : waveJson.at("grid"))
            wd.grid.push_back(row.get<std::string>());

        if (waveJson.contains("spacingX"))         wd.spacingX = waveJson["spacingX"].get<float>();
        if (waveJson.contains("spacingY"))         wd.spacingY = waveJson["spacingY"].get<float>();
        if (waveJson.contains("entryDuration"))    wd.entryDuration = waveJson["entryDuration"].get<float>();
        if (waveJson.contains("entryDelayPerRow")) wd.entryDelayPerRow = waveJson["entryDelayPerRow"].get<float>();
        if (waveJson.contains("formationX"))       wd.formationX = waveJson["formationX"].get<float>();
        if (waveJson.contains("formationY"))       wd.formationY = waveJson["formationY"].get<float>();

        waves.push_back(std::move(wd));
    }
    return waves;
}