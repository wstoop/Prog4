#include "SceneBuilder.h"

#include "SceneManager.h"
#include "ResourceManager.h"
#include "EnemyFactory.h"
#include "Scene.h"
#include "EventManager.h"

#include "Components/TransformComponent.h"
#include "Components/TextComponent.h"
#include "Components/TextureComponent.h"
#include "Components/FormationComponent.h"
#include "Components/EnemyEntryComponent.h"
#include "Components/EntryQueueComponent.h"
#include "Components/ScrollBackgroundComponent.h"
#include "Components/HealthComponent.h"
#include "Components/ShootComponent.h"
#include "Components/HitboxComponent.h"
#include "Components/EnemyFormationSlotComponent.h"
#include "Components/ScoreDisplayComponent.h"
#include "Components/EnemyDataComponent.h"
#include "Components/BulletComponent.h"
#include "Components/LivesComponent.h"
#include "Components/HealthDisplay.h"
#include "Components/PoolComponent.h"

#include "Input/InputManager.h"
#include "Commands/MoveCommand.h"
#include "Commands/ShootCommand.h"
#include "Commands/KillCommand.h"

#include <limits>
#include <cassert>
#include <SDL3/SDL.h>


static const dae::KeyboardAxis k_WASDAxis
{
    SDL_SCANCODE_W, // up
    SDL_SCANCODE_S, // down
    SDL_SCANCODE_A, // left
    SDL_SCANCODE_D // right
};

static void BindPlayer(const SceneInputBinding::PlayerBinding& pb)
{
    auto& input = dae::InputManager::GetInstance();
    dae::GameObject* player = pb.playerPtr;
    const PlayerConfig& cfg = pb.config;
    const uint32_t ctrlID = pb.controllerID;

    switch (cfg.inputType)
    {
    case PlayerInputType::Keyboard:
        input.BindCommand(k_WASDAxis,
            std::make_unique<MoveCommand>(player, cfg.speed));
        input.BindCommand(SDL_SCANCODE_SPACE, dae::KeyState::Down,
            std::make_unique<ShootCommand>(player));
        break;

    case PlayerInputType::Thumbstick:
        input.BindCommand(ctrlID, dae::Thumbstick::Left,
            std::make_unique<MoveCommand>(player, cfg.speed));
        input.BindCommand(ctrlID, dae::ControllerButton::ButtonA, dae::KeyState::Down,
            std::make_unique<ShootCommand>(player));
        break;

    case PlayerInputType::DPad:
        input.BindCommand(ctrlID, dae::Thumbstick::DPad,
            std::make_unique<MoveCommand>(player, cfg.speed));
        input.BindCommand(ctrlID, dae::ControllerButton::ButtonA, dae::KeyState::Down,
            std::make_unique<ShootCommand>(player));
        break;
    }

    if (cfg.inputType == PlayerInputType::Keyboard)
    {
        input.BindCommand(SDL_SCANCODE_X, dae::KeyState::Down,
            std::make_unique<KillCommand>(player));
    }
    else if (ctrlID != UINT32_MAX)
    {
        input.BindCommand(ctrlID, dae::ControllerButton::ButtonX, dae::KeyState::Down,
            std::make_unique<KillCommand>(player));
    }
}

static void UnbindPlayer(const SceneInputBinding::PlayerBinding& pb)
{
    auto& input = dae::InputManager::GetInstance();
    const PlayerConfig& cfg = pb.config;
    const uint32_t ctrlID = pb.controllerID;

    switch (cfg.inputType)
    {
    case PlayerInputType::Keyboard:
        input.UnbindCommand(k_WASDAxis);
        input.UnbindCommand(SDL_SCANCODE_SPACE, dae::KeyState::Down);
        break;

    case PlayerInputType::Thumbstick:
        input.UnbindCommand(ctrlID, dae::Thumbstick::Left);
        input.UnbindCommand(ctrlID, dae::ControllerButton::ButtonA, dae::KeyState::Down);
        break;

    case PlayerInputType::DPad:
        input.UnbindCommand(ctrlID, dae::Thumbstick::DPad);
        input.UnbindCommand(ctrlID, dae::ControllerButton::ButtonA, dae::KeyState::Down);
        break;
    }

    if (cfg.inputType == PlayerInputType::Keyboard)
    {
        input.UnbindCommand(SDL_SCANCODE_X, dae::KeyState::Down);
    }
    else if (ctrlID != UINT32_MAX)
    {
        input.UnbindCommand(ctrlID, dae::ControllerButton::ButtonX, dae::KeyState::Down);
    }
}


void SceneInputBinding::Bind()
{
    for (auto& pb : players)
    {
        if (pb.config.inputType != PlayerInputType::Keyboard && pb.controllerID == UINT32_MAX)
            pb.controllerID = dae::InputManager::GetInstance().AddController();

        BindPlayer(pb);
    }
}

void SceneInputBinding::Unbind()
{
    for (const auto& pb : players)
        UnbindPlayer(pb);
}

void SceneInputBinding::SwitchFrom(SceneInputBinding& previous)
{
    previous.Unbind();
    Bind();
}


SceneBuilder::SceneBuilder(const std::string& sceneName)
    : m_sceneName(sceneName)
{
}

SceneBuilder& SceneBuilder::WithBackground(const std::string& textureFile,
    float screenW, float screenH)
{
    m_backgroundFile = textureFile;
    m_screenW = screenW;
    m_screenH = screenH;
    m_hasBackground = true;
    return *this;
}

SceneBuilder& SceneBuilder::WithEnemies(const std::string& formationFile)
{
    m_formationFile = formationFile;
    m_hasEnemies = true;
    return *this;
}

SceneBuilder& SceneBuilder::WithPlayer(const PlayerConfig& cfg)
{
    m_players.push_back(cfg);
    return *this;
}

SceneBuilder& SceneBuilder::WithHUDForPlayer(int playerIndex)
{
    m_hudPlayerIndices.push_back(playerIndex);
    return *this;
}

BuildResult SceneBuilder::Build()
{
    auto& scene = dae::SceneManager::GetInstance().CreateScene(m_sceneName);

    if (m_hasBackground) SpawnBackground(scene);
    if (m_hasEnemies)    SpawnEnemies(scene);

    BuildResult result;

    for (size_t i = 0; i < m_players.size(); ++i)
    {
        const PlayerConfig& cfg = m_players[i];

        dae::GameObject* ptr = SpawnPlayer(scene, cfg);
        result.playerPtrs.push_back(ptr);

        SceneInputBinding::PlayerBinding pb{ ptr, cfg, UINT32_MAX };
        result.inputBinding.players.push_back(pb);
    }

    for (int idx : m_hudPlayerIndices)
    {
        assert(idx < static_cast<int>(result.playerPtrs.size()) && "HUD player index out of range");
        SpawnHUD(scene, result.playerPtrs[idx], idx);
    }

    return result;
}

void SceneBuilder::SpawnBackground(dae::Scene& scene) const
{
    auto background = std::make_unique<dae::GameObject>();

    auto* bgRoot = background.get();

    auto bgA = std::make_unique<dae::GameObject>();
    bgA->AddComponent<dae::TextureComponent>(m_backgroundFile);
    bgA->GetComponent<dae::TransformComponent>()->SetParent(bgRoot, false);

    auto bgB = std::make_unique<dae::GameObject>();
    bgB->AddComponent<dae::TextureComponent>(m_backgroundFile);
    bgB->GetComponent<dae::TransformComponent>()->SetParent(bgRoot, false);

    background->AddComponent<dae::ScrollBackgroundComponent>(m_screenW, m_screenH);
    scene.Add(std::move(bgA));
    scene.Add(std::move(bgB));
    scene.Add(std::move(background));
}


void SceneBuilder::SpawnEnemies(dae::Scene& scene) const
{
    dae::EnemyFactory::RegisterDefaults();

    const float spacingX = 45.f;
    const float spacingY = 40.f;

    auto formationData = dae::ResourceManager::GetInstance().LoadFormation(m_formationFile);

    auto formationMover = std::make_unique<dae::GameObject>();
    formationMover->GetComponent<dae::TransformComponent>()
        ->SetLocalPosition({ 120.f, 200.f, 0.f });
    formationMover->AddComponent<dae::FormationComponent>();
    auto* formationPtr = formationMover.get();
    auto* formationComp = formationPtr->GetComponent<dae::FormationComponent>();

    int enemyCount = 0;
    std::vector<std::vector<dae::EnemyEntryComponent*>> enemiesByCol(formationData[0].size());

    glm::vec3 formationCenter{};
    {
        glm::vec3 sum{};
        float minY = std::numeric_limits<float>::max();
        int   count = 0;
        for (size_t row = 0; row < formationData.size(); ++row)
            for (size_t col = 0; col < formationData[row].size(); ++col)
            {
                if (formationData[row][col] == '.') continue;
                sum.x += static_cast<float>(col) * spacingX;
                float y = static_cast<float>(row) * spacingY;
                if (y < minY) minY = y;
                ++count;
            }
        if (count > 0)
        {
            formationCenter.x = sum.x / static_cast<float>(count);
            formationCenter.y = minY;
        }
    }

    for (size_t row = 0; row < formationData.size(); ++row)
    {
        for (size_t col = 0; col < formationData[row].size(); ++col)
        {
            char cell = formationData[row][col];
            if (cell == '.') continue;

            auto enemy = dae::EnemyFactory::Create(cell);
            if (!enemy) continue;

            enemy->tag = "Enemy";
            auto* transform = enemy->GetComponent<dae::TransformComponent>();

            glm::vec3 localTarget{
                static_cast<float>(col) * spacingX,
                static_cast<float>(row) * spacingY,
                0.f
            };
            glm::vec3 worldTarget{
                formationPtr->GetComponent<dae::TransformComponent>()->GetLocalPosition().x + localTarget.x,
                formationPtr->GetComponent<dae::TransformComponent>()->GetLocalPosition().y + localTarget.y,
                0.f
            };
            transform->SetScale({ 3.f, 3.f, 0.f });

            const float delay = 0.05f * static_cast<float>(row);
            enemy->AddComponent<dae::EnemyEntryComponent>(
                formationPtr, transform, worldTarget, 2.0f, delay);
            enemy->AddComponent<dae::EnemyFormationSlotComponent>(
                formationComp, localTarget, formationCenter);

            enemiesByCol[col].push_back(enemy->GetComponent<dae::EnemyEntryComponent>());
            ++enemyCount;
            scene.Add(std::move(enemy));
        }
    }

    std::vector<dae::EntryBatch> entryBatches;
    const int totalCols = static_cast<int>(formationData[0].size());
    bool enterFromLeft = false;
    for (int i = 0; i < totalCols / 2; ++i)
    {
        dae::EntryBatch batch;
        for (auto* entry : enemiesByCol[i])
        {
            entry->SetEntryDirection(enterFromLeft);
            batch.enemies.push_back(entry);
        }
        for (auto* entry : enemiesByCol[totalCols - 1 - i])
        {
            entry->SetEntryDirection(!enterFromLeft);
            batch.enemies.push_back(entry);
        }
        entryBatches.push_back(batch);
        enterFromLeft = !enterFromLeft;
    }

    formationMover->AddComponent<dae::EntryQueueComponent>(std::move(entryBatches));
    formationMover->GetComponent<dae::FormationComponent>()->SetAllEnemies(enemyCount);
    scene.Add(std::move(formationMover));
}

dae::GameObject* SceneBuilder::SpawnPlayer(dae::Scene& scene,
    const PlayerConfig& cfg) const
{
    auto player = std::make_unique<dae::GameObject>();

    player->tag = "Player";
    player->AddComponent<dae::TextureComponent>("Player.png");
    player->AddComponent<dae::ShootComponent>(800.f);
    player->AddComponent<dae::HitboxComponent>(15.f * 3, 16.f * 3);
    player->AddComponent<dae::LivesComponent>(cfg.lives);

    auto* health = player->AddComponent<dae::HealthComponent>(cfg.maxHp);
    health->SetDamageFilter([](dae::GameObject*, dae::GameObject* other)
        {
            if (other->tag == "Bullet")
                return other->GetComponent<dae::BulletComponent>()->GetShooter()->tag == "Enemy";
            return other->tag == "Enemy";
        });
    health->RegisterDeathCallback([](dae::GameObject* self)
        {
            auto* lives = self->GetComponent<dae::LivesComponent>();
            lives->LoseLife();
            if (lives->IsGameOver())
            {
                EventManager::GetInstance().SendEvent(GAME_OVER);
                return;
            }
            self->GetComponent<dae::TransformComponent>()->SetLocalPosition({ 300.f, 700.f, 0.f });
            self->GetComponent<dae::HealthComponent>()->Revive();
        });

    player->GetComponent<dae::TransformComponent>()->SetLocalPosition(cfg.spawnPos);
    player->GetComponent<dae::TransformComponent>()->SetScale({ 3.f, 3.f, 0.f });

    dae::GameObject* ptr = player.get();
    scene.Add(std::move(player));
    return ptr;
}

void SceneBuilder::SpawnHUD(dae::Scene& scene, dae::GameObject* playerPtr, int playerIndex) const
{
    auto font = dae::ResourceManager::GetInstance().LoadFont("ArcadeFontSpecialCharacters.ttf", 36);

    const SDL_Color red{ 188, 25,  0,  255 };
    const SDL_Color white{ 255, 255, 255, 255 };

    const float colX = 650.f;
    const float scoreY = 180.f + playerIndex * 100.f;

    const float startX = 630.f;
    const float iconGap = 50.f;
    const float baseY = 380.f + playerIndex * 60.f;

    auto AddLabel = [&](const std::string& text, SDL_Color color, glm::vec3 pos)
        {
            auto go = std::make_unique<dae::GameObject>();
            go->GetComponent<dae::TransformComponent>()->SetLocalPosition(pos);
            go->AddComponent<dae::TextComponent>(text, font, color);
            scene.Add(std::move(go));
        };

    if (playerIndex == 0)
    {
        AddLabel("P1: WASD to move / Space to shoot", white, { 30, 30, 0 });
        AddLabel("P2: DPAD to move / A to shoot", white, { 30, 60, 0 });
        AddLabel("press X (kb and ctrlr respectfullly) to die", white, { 30, 110, 0 });
        AddLabel("(can also die by contact with enemy)", white, { 30, 140, 0 });

        AddLabel("HIGH", red, { 650,  75, 0 });
        AddLabel("SCORE", red, { 670,  95, 0 });
        AddLabel("30000", white, { 670, 115, 0 });
    }

    const std::string upLabel = std::to_string(playerIndex + 1) + "UP";
    AddLabel(upLabel, red, { colX, scoreY, 0 });
    {
        auto scoreGO = std::make_unique<dae::GameObject>();
        scoreGO->GetComponent<dae::TransformComponent>()->SetLocalPosition({ colX, scoreY + 40.f, 0 });
        scoreGO->AddComponent<dae::TextComponent>("00", font, white);
        scoreGO->AddComponent<dae::ScoreDisplayComponent>(playerPtr);
        scene.Add(std::move(scoreGO));
    }

    auto livesDisplay = std::make_unique<dae::GameObject>();
    livesDisplay->AddComponent<dae::HealthDisplay>(playerPtr);

    const int livesCount = playerPtr->GetComponent<dae::LivesComponent>()->GetLives();
    for (int i = 0; i < livesCount - 1; ++i)
    {
        auto icon = std::make_unique<dae::GameObject>();
        icon->GetComponent<dae::TransformComponent>()
            ->SetLocalPosition({ startX + i * iconGap, baseY, 0 });
        icon->GetComponent<dae::TransformComponent>()->SetScale({ 3, 3, 0 });
        icon->AddComponent<dae::TextureComponent>("Player.png");
        icon->GetComponent<dae::TransformComponent>()->SetParent(livesDisplay.get(), false);
        scene.Add(std::move(icon));
    }
    scene.Add(std::move(livesDisplay));

    if (playerIndex == 0)
    {
        auto levelIcon = std::make_unique<dae::GameObject>();
        levelIcon->GetComponent<dae::TransformComponent>()
            ->SetLocalPosition({ startX, baseY + 60.f * 4, 0 });
        levelIcon->GetComponent<dae::TransformComponent>()->SetScale({ 3, 3, 0 });
        levelIcon->AddComponent<dae::TextureComponent>("levelCounter.png");
        scene.Add(std::move(levelIcon));
    }
}