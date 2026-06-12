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

#include "Components/ScrollBackgroundComponent.h"
#include "Components/PlayerHealthComponent.h"
#include "Components/ShootComponent.h"
#include "Components/HitboxComponent.h"
#include "Components/EnemyFormationSlotComponent.h"
#include "Components/ScoreDisplayComponent.h"
#include "Components/HighScoreDisplayComponent.h"
#include "Components/LevelDisplayComponent.h"
#include "Components/EnemyDataComponent.h"
#include "Components/TractorBeamComponent.h"
#include "Components/BulletComponent.h"
#include "Components/LivesComponent.h"
#include "Components/HealthDisplay.h"
#include "Components/PoolComponent.h"
#include "Components/SelectableButtonComponent.h"
#include "Components/WaveManager.h"
#include "Components/CapturedFighterSpawner.h"
#include "States/Enemies/EnemyBrainComponent.h"

#include "Input/InputManager.h"
#include "Commands/MoveCommand.h"
#include "Commands/ShootCommand.h"
#include "Commands/KillCommand.h"
#include "Commands/UIMoveCommand.h"
#include "Commands/UIMoveButtonCommand.h"
#include "Commands/UIConfirmCommand.h"
#include "Commands/BossDiveBombCommand.h"
#include "Commands/BossTractorBeamCommand.h"
#include "Components/BossPlayerControlComponent.h"
#include <limits>
#include <cassert>
#include <set>
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

    if (pb.controlsBoss)
    {
        input.BindCommand(ctrlID, dae::ControllerButton::ButtonA, dae::KeyState::Down,
            std::make_unique<BossDiveBombCommand>(player));
        input.BindCommand(ctrlID, dae::ControllerButton::ButtonB, dae::KeyState::Down,
            std::make_unique<BossTractorBeamCommand>(player));
        return;
    }

    switch (cfg.inputType)
    {
    case PlayerInputType::Keyboard:
        input.BindCommand(k_WASDAxis, dae::KeyState::Pressed,
            std::make_unique<MoveCommand>(player, cfg.speed));
        input.BindCommand(SDL_SCANCODE_SPACE, dae::KeyState::Down,
            std::make_unique<ShootCommand>(player));

        if (ctrlID != UINT32_MAX)
        {
            input.BindCommand(ctrlID, dae::Thumbstick::DPad,
                std::make_unique<MoveCommand>(player, cfg.speed));
            input.BindCommand(ctrlID, dae::ControllerButton::ButtonA, dae::KeyState::Down,
                std::make_unique<ShootCommand>(player));
        }
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

    if (ctrlID != UINT32_MAX)
    {
        input.BindCommand(ctrlID, dae::ControllerButton::ButtonX, dae::KeyState::Down,
            std::make_unique<KillCommand>(player));
    }

    //input.BindCommand(SDL_SCANCODE_ESCAPE, dae::KeyState::Down,
    //    std::make_unique<dae::PauseGameCommand>());
}

static void UnbindPlayer(const SceneInputBinding::PlayerBinding& pb)
{
    auto& input = dae::InputManager::GetInstance();
    const PlayerConfig& cfg = pb.config;
    const uint32_t ctrlID = pb.controllerID;

    if (pb.controlsBoss)
    {
        input.UnbindCommand(ctrlID, dae::ControllerButton::ButtonA, dae::KeyState::Down);
        input.UnbindCommand(ctrlID, dae::ControllerButton::ButtonB, dae::KeyState::Down);
        return;
    }

    switch (cfg.inputType)
    {
    case PlayerInputType::Keyboard:
        input.UnbindCommand(k_WASDAxis, dae::KeyState::Pressed);
        input.UnbindCommand(SDL_SCANCODE_SPACE, dae::KeyState::Down);

        if (ctrlID != UINT32_MAX)
        {
            input.UnbindCommand(ctrlID, dae::Thumbstick::DPad);
            input.UnbindCommand(ctrlID, dae::ControllerButton::ButtonA, dae::KeyState::Down);
        }
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

    if (ctrlID != UINT32_MAX)
    {
        input.UnbindCommand(ctrlID, dae::ControllerButton::ButtonX, dae::KeyState::Down);
    }

    //input.UnbindCommand(SDL_SCANCODE_ESCAPE, dae::KeyState::Down);
}


void SceneInputBinding::Bind()
{
    // First pass: players that explicitly use a controller claim one.
    std::set<uint32_t> usedControllers;
    for (auto& pb : players)
    {
        if (pb.config.inputType != PlayerInputType::Keyboard)
        {
            if (pb.controllerID == UINT32_MAX)
                pb.controllerID = dae::InputManager::GetInstance().AddController(usedControllers);
            if (pb.controllerID != UINT32_MAX)
                usedControllers.insert(pb.controllerID);
        }
    }

    // Second pass: keyboard players also get a free controller as a fallback,
    // so a single keyboard-bound player can play with either device.
    for (auto& pb : players)
    {
        if (pb.config.inputType == PlayerInputType::Keyboard && pb.controllerID == UINT32_MAX)
        {
            uint32_t ctrlID = dae::InputManager::GetInstance().AddController(usedControllers);
            if (ctrlID != UINT32_MAX)
            {
                pb.controllerID = ctrlID;
                usedControllers.insert(ctrlID);
            }
        }
    }

    for (auto& pb : players)
        BindPlayer(pb);

    for (auto& fn : onBind) fn();
}

void SceneInputBinding::Unbind()
{
    for (const auto& pb : players)
        UnbindPlayer(pb);

    for (auto& fn : onUnbind) fn();
}

void SceneInputBinding::SwitchFrom(SceneInputBinding& previous)
{
    previous.Unbind();
    Bind();

    std::for_each(players.begin(), players.end(), [this](PlayerBinding& pb) {
        pb.config.lives = 3;
		});
}


SceneBuilder::SceneBuilder(const std::string& sceneName)
    : m_sceneName(sceneName)
{}

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

SceneBuilder& SceneBuilder::WithPlayerControlledBoss()
{
    m_playerControlledBoss = true;
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

    BuildResult result;
    bool hasPlayer = false;
    result.menuStartingButton = m_pMenuStartingButton;

    for (size_t i = 0; i < m_players.size(); ++i)
    {
        const PlayerConfig& cfg = m_players[i];

        dae::GameObject* ptr = SpawnPlayer(scene, cfg);
        result.playerPtrs.push_back(ptr);

        SceneInputBinding::PlayerBinding pb{ ptr, cfg, UINT32_MAX };
        result.inputBinding.players.push_back(pb);

        hasPlayer = true;
    }

    if (hasPlayer)
    {
        auto poolGO = std::make_unique<dae::GameObject>();
        poolGO->AddComponent<dae::PoolComponent>();
        scene.Add(std::move(poolGO));
    }

    // Death/respawn handling: a player respawns if they have lives left.
    // Game over only fires once every player has run out of lives.
    {
        const auto allPlayers = result.playerPtrs;
        for (size_t i = 0; i < allPlayers.size(); ++i)
        {
            dae::GameObject* player = allPlayers[i];
            const glm::vec3 spawnPos = m_players[i].spawnPos;

            player->GetComponent<dae::PlayerHealthComponent>()->RegisterDeathCallback(
                [allPlayers, spawnPos](dae::GameObject* owner)
                {
                    auto* lives = owner->GetComponent<dae::LivesComponent>();
                    lives->LoseLife();

                    if (lives->IsGameOver())
                    {
                        // All players that died in this batch get their death
                        // callback fired in turn, so a co-op partner who is
                        // also dying right now may not have called LoseLife()
                        // yet. Predict their post-decrement life count instead
                        // of reading their (possibly stale) current value, so
                        // the order in which callbacks run doesn't matter.
                        bool anyoneLeft = false;
                        for (auto* other : allPlayers)
                        {
                            if (other == owner) continue;

                            auto* otherLives = other->GetComponent<dae::LivesComponent>();
                            int remaining = otherLives->GetLives();
                            if (other->GetComponent<dae::PlayerHealthComponent>()->IsDead())
                                --remaining;

                            if (remaining > 0)
                            {
                                anyoneLeft = true;
                                break;
                            }
                        }

                        if (!anyoneLeft)
                            EventManager::GetInstance().SendEvent(EVENT_GAME_OVER);

                        // This player is permanently out (no lives left) but
                        // their co-op partner is still playing - stay dead
                        // and off-screen (Die() already moved them) rather
                        // than reviving into an invisible "ghost" that can
                        // still move/shoot.
                        return;
                    }

                    owner->GetComponent<dae::TransformComponent>()->SetLocalPosition(spawnPos);
                    owner->GetComponent<dae::PlayerHealthComponent>()->Revive();
                });
        }
    }

    if (m_playerControlledBoss)
    {
        SpawnPlayerControlledBoss(scene);
        result.bossPtr = m_pBossPtr;

        SceneInputBinding::PlayerBinding bossBinding{};
        bossBinding.playerPtr = m_pBossPtr;
        bossBinding.config.inputType = PlayerInputType::Thumbstick;
        bossBinding.controlsBoss = true;
        result.inputBinding.players.push_back(bossBinding);
    }

    for (int idx : m_hudPlayerIndices)
    {
        assert(idx < static_cast<int>(result.playerPtrs.size()) && "HUD player index out of range");
        SpawnHUD(scene, result.playerPtrs[idx], idx);
    }

    if (m_hasMenuButtons)
    {
        SpawnMenuButtons(scene, result.inputBinding);
    }
    if (m_hasNextButton)
    {
        SpawnNextButton(scene, result.inputBinding);
    }
    if (m_hasEnemies)
    {
        auto waveManagerGO = std::make_unique<dae::GameObject>();
        waveManagerGO->AddComponent<dae::WaveManager>(scene, "waves.json");
        auto* waveManager = waveManagerGO->GetComponent<dae::WaveManager>();
        scene.Add(std::move(waveManagerGO));

        waveManager->StartFirstWave();

        auto capturedFighterSpawnerGO = std::make_unique<dae::GameObject>();
        capturedFighterSpawnerGO->AddComponent<dae::CapturedFighterSpawner>(scene);
        scene.Add(std::move(capturedFighterSpawnerGO));
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
    const int totalCols = static_cast<int>(formationData[0].size());

    // Compute formation center for sway offset calculations
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

    // Determine entry direction per column (mirrors original EntryQueueComponent logic)
    std::vector<bool> colFromLeft(totalCols, true);
    {
        bool enterFromLeft = false;
        for (int i = 0; i < totalCols / 2; ++i)
        {
            colFromLeft[i] = enterFromLeft;
            colFromLeft[totalCols - 1 - i] = !enterFromLeft;
            enterFromLeft = !enterFromLeft;
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
            transform->SetScale({ 3.f, 3.f, 0.f });

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

            const float delay = 0.05f * static_cast<float>(row);
            const bool  fromLeft = colFromLeft[col];

            enemy->AddComponent<dae::EnemyFormationSlotComponent>(
                formationComp, localTarget, formationCenter);

            auto* brain = enemy->AddComponent<dae::EnemyBrainComponent>();
            brain->SetEntryConfig(worldTarget, 2.0f, delay, fromLeft, formationPtr);

            ++enemyCount;
            scene.Add(std::move(enemy));
        }
    }

    formationMover->GetComponent<dae::FormationComponent>()->SetAllEnemies(enemyCount);
    scene.Add(std::move(formationMover));
}

void SceneBuilder::SpawnPlayerControlledBoss(dae::Scene& scene) const
{
    dae::EnemyFactory::RegisterDefaults();

    const glm::vec3 bossPos{ 270.f, 150.f, 0.f };

    auto formationMover = std::make_unique<dae::GameObject>();
    formationMover->GetComponent<dae::TransformComponent>()->SetLocalPosition(bossPos);
    formationMover->AddComponent<dae::FormationComponent>();
    auto* formationPtr = formationMover.get();
    auto* formationComp = formationPtr->GetComponent<dae::FormationComponent>();
    formationComp->SetReportsWaveCleared(false);

    auto boss = dae::EnemyFactory::Create('G');
    boss->tag = "Enemy";
    boss->GetComponent<dae::TransformComponent>()->SetScale({ 3.f, 3.f, 0.f });

    boss->AddComponent<dae::EnemyFormationSlotComponent>(
        formationComp, glm::vec3{ 0.f, 0.f, 0.f }, glm::vec3{ 0.f, 0.f, 0.f });

    auto* brain = boss->AddComponent<dae::EnemyBrainComponent>();
    brain->SetEntryConfig(bossPos, 2.0f, 0.f, true, formationPtr);

    // Player-controlled: never auto-attacks, attacks are triggered by Player 2.
    boss->GetComponent<dae::EnemyDataComponent>()->SetCombatStateFactory(nullptr);
    boss->AddComponent<dae::BossPlayerControlComponent>();

    formationComp->SetAllEnemies(1);

    m_pBossPtr = boss.get();

    scene.Add(std::move(boss));
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

    auto* health = player->AddComponent<dae::PlayerHealthComponent>(cfg.maxHp);
    health->SetDamageFilter([](dae::GameObject*, dae::GameObject* other)
        {
            if (other->tag == "Bullet")
                return other->GetComponent<dae::BulletComponent>()->GetShooter()->tag == "Enemy";

            // A player-controlled boss has a TractorBeamComponent - its body
            // shouldn't deal collision damage to the player (which would
            // teleport them away mid-frame and prevent the beam from ever
            // registering a capture). Only its tractor beam should affect
            // the player.
            if (other->tag == "Enemy" && other->GetComponent<dae::TractorBeamComponent>() != nullptr)
                return false;

            return other->tag == "Enemy";
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
        AddLabel("HIGH", red, { 650,  75, 0 });
        AddLabel("SCORE", red, { 670,  95, 0 });

        auto highScoreGO = std::make_unique<dae::GameObject>();
        highScoreGO->GetComponent<dae::TransformComponent>()->SetLocalPosition({ 670, 115, 0 });
        highScoreGO->AddComponent<dae::TextComponent>("0", font, white);
        highScoreGO->AddComponent<dae::HighScoreDisplayComponent>();
        scene.Add(std::move(highScoreGO));
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

        auto levelDisplay = std::make_unique<dae::GameObject>();
        levelDisplay->GetComponent<dae::TransformComponent>()
            ->SetLocalPosition({ startX + 40.f, baseY + 60.f * 4 - 10.f, 0 });
        levelDisplay->AddComponent<dae::TextComponent>("1", font, white);
        levelDisplay->AddComponent<dae::LevelDisplayComponent>();
        scene.Add(std::move(levelDisplay));
    }
}

SceneBuilder& SceneBuilder::WithMenuButtons()
{
    m_hasMenuButtons = true;
    return *this;

}

SceneBuilder& SceneBuilder::WithNextButton(EventId nextStateEventId)
{
    m_hasNextButton = true;
    m_nextButtonEventId = nextStateEventId;
    return *this;
}
void SceneBuilder::SpawnMenuButtons(dae::Scene& scene, SceneInputBinding& binding)
{
    auto font = dae::ResourceManager::GetInstance().LoadFont("ArcadeFontSpecialCharacters.ttf", 36);
    const SDL_Color white{ 255, 255, 255, 255 };

    auto goSingle = std::make_unique<dae::GameObject>();
    auto goCoOp = std::make_unique<dae::GameObject>();
    auto goVersus = std::make_unique<dae::GameObject>();

    goSingle->GetComponent<dae::TransformComponent>()->SetLocalPosition({ 300, 300, 0 });
    goCoOp->GetComponent<dae::TransformComponent>()->SetLocalPosition({ 300, 360, 0 });
    goVersus->GetComponent<dae::TransformComponent>()->SetLocalPosition({ 300, 420, 0 });

    goSingle->AddComponent<dae::TextComponent>("Single Player", font, white);
    goCoOp->AddComponent<dae::TextComponent>("Co-Op", font, white);
    goVersus->AddComponent<dae::TextComponent>("Versus", font, white);

    // Wire neighbors
    goSingle->AddComponent<dae::SelectableButtonComponent>(
        dae::SelectableButtonComponent::ConnectedUIElements{ nullptr, goCoOp.get(), nullptr, nullptr });
    goCoOp->AddComponent<dae::SelectableButtonComponent>(
        dae::SelectableButtonComponent::ConnectedUIElements{ goSingle.get(), goVersus.get(), nullptr, nullptr });
    goVersus->AddComponent<dae::SelectableButtonComponent>(
        dae::SelectableButtonComponent::ConnectedUIElements{ goCoOp.get(), nullptr, nullptr, nullptr });

    // Select the first one by default
    goSingle->GetComponent<dae::SelectableButtonComponent>()->Select();
    m_pMenuStartingButton = goSingle.get();

    goSingle->GetComponent<dae::SelectableButtonComponent>()->SetOnActivate(
        [] { EventManager::GetInstance().SendEvent(EVENT_LOAD_SINGLE); });
    goCoOp->GetComponent<dae::SelectableButtonComponent>()->SetOnActivate(
        [] { EventManager::GetInstance().SendEvent(EVENT_LOAD_COOP); });
    goVersus->GetComponent<dae::SelectableButtonComponent>()->SetOnActivate(
        [] { EventManager::GetInstance().SendEvent(EVENT_LOAD_VERSUS); });
    
    scene.Add(std::move(goSingle));
    scene.Add(std::move(goCoOp));
    scene.Add(std::move(goVersus));

    auto sel = std::make_unique<UISelection>(m_pMenuStartingButton);
    UISelection* selPtr = sel.get();
    binding.ownedSelections.push_back(std::move(sel));

    binding.onBind.push_back([selPtr]() {
        auto& input = dae::InputManager::GetInstance();
        input.BindCommand(k_WASDAxis, dae::KeyState::Down, std::make_unique<UIMoveCommand>(selPtr));
        input.BindCommand(SDL_SCANCODE_SPACE, dae::KeyState::Down,
            std::make_unique<UIConfirmCommand>(selPtr));

        uint32_t ctrlID = input.AddController();
        if (ctrlID != UINT32_MAX)
        {
            input.BindCommand(ctrlID, dae::ControllerButton::DpadUp, dae::KeyState::Down,
                std::make_unique<UIMoveButtonCommand>(selPtr, glm::vec2{ 0.f, 1.f }));
            input.BindCommand(ctrlID, dae::ControllerButton::DpadDown, dae::KeyState::Down,
                std::make_unique<UIMoveButtonCommand>(selPtr, glm::vec2{ 0.f, -1.f }));
            input.BindCommand(ctrlID, dae::ControllerButton::DpadLeft, dae::KeyState::Down,
                std::make_unique<UIMoveButtonCommand>(selPtr, glm::vec2{ -1.f, 0.f }));
            input.BindCommand(ctrlID, dae::ControllerButton::DpadRight, dae::KeyState::Down,
                std::make_unique<UIMoveButtonCommand>(selPtr, glm::vec2{ 1.f, 0.f }));
            input.BindCommand(ctrlID, dae::ControllerButton::ButtonA, dae::KeyState::Down,
                std::make_unique<UIConfirmCommand>(selPtr));
        }
        });

    binding.onUnbind.push_back([]() {
        auto& input = dae::InputManager::GetInstance();
        input.UnbindCommand(k_WASDAxis, dae::KeyState::Down);
        input.UnbindCommand(SDL_SCANCODE_SPACE, dae::KeyState::Down);

        uint32_t ctrlID = input.AddController();
        if (ctrlID != UINT32_MAX)
        {
            input.UnbindCommand(ctrlID, dae::ControllerButton::DpadUp, dae::KeyState::Down);
            input.UnbindCommand(ctrlID, dae::ControllerButton::DpadDown, dae::KeyState::Down);
            input.UnbindCommand(ctrlID, dae::ControllerButton::DpadLeft, dae::KeyState::Down);
            input.UnbindCommand(ctrlID, dae::ControllerButton::DpadRight, dae::KeyState::Down);
            input.UnbindCommand(ctrlID, dae::ControllerButton::ButtonA, dae::KeyState::Down);
        }
        });
}
void SceneBuilder::SpawnNextButton(dae::Scene& scene, SceneInputBinding& binding)
{
    auto font = dae::ResourceManager::GetInstance().LoadFont("ArcadeFontSpecialCharacters.ttf", 28);
    const SDL_Color white{ 255, 255, 255, 255 };

    auto goNext = std::make_unique<dae::GameObject>();
    // Positioned near the bottom center of your 600x830 play space
    goNext->GetComponent<dae::TransformComponent>()->SetLocalPosition({ 160.f, 740.f, 0.f });
    goNext->AddComponent<dae::TextComponent>("PRESS SHOOT TO CONTINUE", font, white);

    // Wire neighbors (isolated single button node)
    goNext->AddComponent<dae::SelectableButtonComponent>(
        dae::SelectableButtonComponent::ConnectedUIElements{ nullptr, nullptr, nullptr, nullptr });

    // Select the button by default (Matches SpawnMenuButtons structure)
    goNext->GetComponent<dae::SelectableButtonComponent>()->Select();

    // Cache the custom hashed EventId safely for the capture block
    EventId cachedEventId = m_nextButtonEventId;
    goNext->GetComponent<dae::SelectableButtonComponent>()->SetOnActivate([cachedEventId]() {
        EventManager::GetInstance().SendEvent(cachedEventId);
        });

    dae::GameObject* buttonPtr = goNext.get();
    scene.Add(std::move(goNext));

    // Register active selection tracking context layout
    auto sel = std::make_unique<UISelection>(buttonPtr);
    UISelection* selPtr = sel.get();
    binding.ownedSelections.push_back(std::move(sel));

    binding.onBind.push_back([selPtr]() {
        auto& input = dae::InputManager::GetInstance();
        // Bind axis movement tracking commands (Matches SpawnMenuButtons)
        input.BindCommand(k_WASDAxis, dae::KeyState::Down, std::make_unique<UIMoveCommand>(selPtr));
        // Bind selection activation trigger execution command
        input.BindCommand(SDL_SCANCODE_SPACE, dae::KeyState::Down,
            std::make_unique<UIConfirmCommand>(selPtr));

        uint32_t ctrlID = input.AddController();
        if (ctrlID != UINT32_MAX)
        {
            input.BindCommand(ctrlID, dae::ControllerButton::DpadUp, dae::KeyState::Down,
                std::make_unique<UIMoveButtonCommand>(selPtr, glm::vec2{ 0.f, 1.f }));
            input.BindCommand(ctrlID, dae::ControllerButton::DpadDown, dae::KeyState::Down,
                std::make_unique<UIMoveButtonCommand>(selPtr, glm::vec2{ 0.f, -1.f }));
            input.BindCommand(ctrlID, dae::ControllerButton::DpadLeft, dae::KeyState::Down,
                std::make_unique<UIMoveButtonCommand>(selPtr, glm::vec2{ -1.f, 0.f }));
            input.BindCommand(ctrlID, dae::ControllerButton::DpadRight, dae::KeyState::Down,
                std::make_unique<UIMoveButtonCommand>(selPtr, glm::vec2{ 1.f, 0.f }));
            input.BindCommand(ctrlID, dae::ControllerButton::ButtonA, dae::KeyState::Down,
                std::make_unique<UIConfirmCommand>(selPtr));
        }
        });

    binding.onUnbind.push_back([]() {
        auto& input = dae::InputManager::GetInstance();
        input.UnbindCommand(k_WASDAxis, dae::KeyState::Down);
        input.UnbindCommand(SDL_SCANCODE_SPACE, dae::KeyState::Down);

        uint32_t ctrlID = input.AddController();
        if (ctrlID != UINT32_MAX)
        {
            input.UnbindCommand(ctrlID, dae::ControllerButton::DpadUp, dae::KeyState::Down);
            input.UnbindCommand(ctrlID, dae::ControllerButton::DpadDown, dae::KeyState::Down);
            input.UnbindCommand(ctrlID, dae::ControllerButton::DpadLeft, dae::KeyState::Down);
            input.UnbindCommand(ctrlID, dae::ControllerButton::DpadRight, dae::KeyState::Down);
            input.UnbindCommand(ctrlID, dae::ControllerButton::ButtonA, dae::KeyState::Down);
        }
        });
}