#include "GameState.h"
#include "GameStateManager.h"
#include "ResourceManager.h"
#include "Components/TransformComponent.h"
#include "Components/TextComponent.h"

namespace dae
{
    // ---- MenuState ----

    MenuState::MenuState(GameStateManager& m) : GameState(m)
    {
        m_binding = SceneBuilder("Menu")
            .WithBackground("Background_Galaga.png")
            .WithMenuButtons()
            .Build()
            .inputBinding;
    }

    void MenuState::OnEnter(SceneInputBinding& previous)
    {
        m_binding.SwitchFrom(previous);
        SceneManager::GetInstance().SetActiveScene("Menu");
    }

    void MenuState::OnExit()
    {
        auto& data = m_manager.GetSharedData();
        data.Score = 0;
        data.ShotsFired = 0;
        data.ShotsHit = 0;
    }

    // ---- SinglePlayerState ----

    SinglePlayerState::SinglePlayerState(GameStateManager& m) : GameState(m)
    {
    }

    void SinglePlayerState::OnEnter(SceneInputBinding& previous)
    {
        SceneManager::GetInstance().RemoveScene("SinglePlayer");
		EventManager::GetInstance().ClearQueue();
        m_manager.ResetSharedData();
        m_binding = SceneBuilder("SinglePlayer")
            .WithBackground("Background_Galaga.png")
            .WithEnemies()
            .WithPlayer(PlayerConfig::Keyboard(100.f))
            .WithHUDForPlayer(0)
            .Build()
            .inputBinding;

        m_binding.SwitchFrom(previous);
        SceneManager::GetInstance().SetActiveScene("SinglePlayer");
    }

    // ---- CoOpState ----

    CoOpState::CoOpState(GameStateManager& m) : GameState(m)
    {

    }

    void CoOpState::OnEnter(SceneInputBinding& previous)
    {
        SceneManager::GetInstance().RemoveScene("CoOp");
        EventManager::GetInstance().ClearQueue();
        m_manager.ResetSharedData();
        m_binding = SceneBuilder("CoOp")
            .WithBackground("Background_Galaga.png")
            .WithEnemies()
            .WithPlayer(PlayerConfig::Keyboard(100.f, { 200.f, 700.f, 0.f }))
            .WithPlayer(PlayerConfig::Controller(PlayerInputType::DPad, 200.f, { 400.f, 700.f, 0.f }))
            .WithHUDForPlayer(0)
            .WithHUDForPlayer(1)
            .Build()
            .inputBinding;

        m_binding.SwitchFrom(previous);
        SceneManager::GetInstance().SetActiveScene("CoOp");
    }

    // ---- VersusState ----

    VersusState::VersusState(GameStateManager& m) : GameState(m)
    {

    }

    void VersusState::OnEnter(SceneInputBinding& previous)
    {
        SceneManager::GetInstance().RemoveScene("Versus");
        EventManager::GetInstance().ClearQueue();
        m_manager.ResetSharedData();
        m_binding = SceneBuilder("Versus")
            .WithBackground("Background_Galaga.png")
            .WithEnemies()
            .WithPlayer(PlayerConfig::Controller(PlayerInputType::Thumbstick, 100.f))
            .Build()
            .inputBinding;

        m_binding.SwitchFrom(previous);
        SceneManager::GetInstance().SetActiveScene("Versus");
    }

    // ---- EndScreenState ----

    EndScreenState::EndScreenState(GameStateManager& m) : GameState(m)
    {
        // 1. Build the base EndScreen scene
        m_binding = SceneBuilder("EndScreen")
            .WithBackground("Background_Galaga.png")
            .WithNextButton(EVENT_LOAD_HIGHSCORE)
            .Build()
            .inputBinding;
    }

    void EndScreenState::OnEnter(SceneInputBinding& previous)
    {
        auto& data = m_manager.GetSharedData();

        float accuracy = (data.ShotsFired > 0)
            ? (static_cast<float>(data.ShotsHit) / data.ShotsFired) * 100.f
            : 0.f;

        char accuracyBuf[16];
        std::snprintf(accuracyBuf, sizeof(accuracyBuf), "%.2f%%", accuracy);

        auto scene = SceneManager::GetInstance().GetScene("EndScreen");
        auto font = ResourceManager::GetInstance().LoadFont("ArcadeFontSpecialCharacters.ttf", 36);

        const SDL_Color red{ 188,  25,   0, 255 };
        const SDL_Color white{ 255, 255, 255, 255 };
        const float centerX = 300.f;

        auto AddLabel = [&](const std::string& text, SDL_Color color, float y)
            {
                auto go = std::make_unique<GameObject>();
                go->GetComponent<TransformComponent>()->SetLocalPosition({ centerX, y, 0.f });
                go->AddComponent<TextComponent>(text, font, color);
                scene->Add(std::move(go));
            };

        AddLabel("GAME OVER", red, 200.f);
        AddLabel("SCORE", red, 290.f);
        AddLabel(std::to_string(data.Score), white, 330.f);
        AddLabel("SHOTS FIRED", red, 390.f);
        AddLabel(std::to_string(data.ShotsFired), white, 430.f);
        AddLabel("SHOTS HIT: ", red, 470.f);
        AddLabel(std::to_string(data.ShotsHit), white, 510.f);
        AddLabel("ACCURACY", red, 570.f);
        AddLabel(accuracyBuf, white, 610.f);

        m_binding.SwitchFrom(previous);
        SceneManager::GetInstance().SetActiveScene("EndScreen");
    }

    HighScoreState::HighScoreState(GameStateManager& m) : GameState(m)
    {
        m_binding = SceneBuilder("HighScore")
            .WithBackground("Background_Galaga.png")
            .WithNextButton(EVENT_LOAD_START)
            .Build()
            .inputBinding;
    }

    void HighScoreState::OnEnter(SceneInputBinding& previous)
    {
        m_binding.SwitchFrom(previous);
        SceneManager::GetInstance().SetActiveScene("HighScore");
    }

}