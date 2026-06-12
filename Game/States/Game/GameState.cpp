#include "GameState.h"
#include "GameStateManager.h"
#include "ResourceManager.h"
#include "Components/TransformComponent.h"
#include "Components/TextComponent.h"
#include "Components/HighScoreManager.h"
#include "Components/NameEntryComponent.h"
#include "Commands/NameEntryAxisCommand.h"
#include "Commands/NameEntryButtonCommand.h"
#include "Commands/NameEntryConfirmCommand.h"
#include "Input/InputManager.h"
#include <SDL3/SDL.h>

namespace
{
    const dae::KeyboardAxis kHighScoreAxis
    {
        SDL_SCANCODE_W,
        SDL_SCANCODE_S,
        SDL_SCANCODE_A,
        SDL_SCANCODE_D
    };
}

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
            .WithPlayerControlledBoss()
            .WithPlayer(PlayerConfig::Keyboard(100.f))
            .WithHUDForPlayer(0)
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

        for (auto* label : m_labels)
            label->m_destroy = true;
        m_labels.clear();

        auto AddLabel = [&](const std::string& text, SDL_Color color, float y)
            {
                auto go = std::make_unique<GameObject>();
                go->GetComponent<TransformComponent>()->SetLocalPosition({ centerX, y, 0.f });
                go->AddComponent<TextComponent>(text, font, color);
                m_labels.push_back(go.get());
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

        m_scene = SceneManager::GetInstance().GetScene("HighScore");

        for (auto* go : m_nameEntryObjects)
            go->m_destroy = true;
        m_nameEntryObjects.clear();
        m_nameEntry = nullptr;

        for (auto* label : m_labels)
            label->m_destroy = true;
        m_labels.clear();

        RebuildScoreList();

        auto& data = m_manager.GetSharedData();
        if (data.Score > 0 && HighScoreManager::Qualifies(data.Score))
            StartNameEntry(data.Score);

        SceneManager::GetInstance().SetActiveScene("HighScore");
    }

    void HighScoreState::RebuildScoreList()
    {
        auto font = ResourceManager::GetInstance().LoadFont("ArcadeFontSpecialCharacters.ttf", 28);
        const SDL_Color red{ 188,  25,   0, 255 };
        const SDL_Color white{ 255, 255, 255, 255 };

        auto AddLabel = [&](const std::string& text, SDL_Color color, float y)
            {
                auto go = std::make_unique<GameObject>();
                go->GetComponent<TransformComponent>()->SetLocalPosition({ 150.f, y, 0.f });
                go->AddComponent<TextComponent>(text, font, color);
                m_labels.push_back(go.get());
                m_scene->Add(std::move(go));
            };

        AddLabel("HIGH SCORES", red, 80.f);

        float y = 140.f;
        int rank = 1;
        for (const auto& entry : HighScoreManager::Load())
        {
            AddLabel(std::to_string(rank) + ". " + entry.name + "  " + std::to_string(entry.score), white, y);
            ++rank;
            y += 40.f;
        }
    }

    void HighScoreState::StartNameEntry(int score)
    {
        auto font = ResourceManager::GetInstance().LoadFont("ArcadeFontSpecialCharacters.ttf", 36);
        const SDL_Color red{ 188,  25,   0, 255 };
        const SDL_Color white{ 255, 255, 255, 255 };

        auto prompt = std::make_unique<GameObject>();
        prompt->GetComponent<TransformComponent>()->SetLocalPosition({ 100.f, 600.f, 0.f });
        prompt->AddComponent<TextComponent>("NEW HIGH SCORE! ENTER YOUR NAME", font, red);
        m_nameEntryObjects.push_back(prompt.get());
        m_scene->Add(std::move(prompt));

        std::vector<TextComponent*> letterTexts;
        const float startX = 250.f;
        for (int i = 0; i < 3; ++i)
        {
            auto letterGO = std::make_unique<GameObject>();
            letterGO->GetComponent<TransformComponent>()->SetLocalPosition({ startX + i * 40.f, 660.f, 0.f });
            letterGO->AddComponent<TextComponent>("A", font, white);
            letterTexts.push_back(letterGO->GetComponent<TextComponent>());
            m_nameEntryObjects.push_back(letterGO.get());
            m_scene->Add(std::move(letterGO));
        }

        auto entryGO = std::make_unique<GameObject>();
        entryGO->AddComponent<NameEntryComponent>(letterTexts);
        m_nameEntry = entryGO->GetComponent<NameEntryComponent>();
        m_nameEntryObjects.push_back(entryGO.get());
        m_scene->Add(std::move(entryGO));

        auto& input = InputManager::GetInstance();
        input.UnbindCommand(kHighScoreAxis, KeyState::Down);
        input.UnbindCommand(SDL_SCANCODE_SPACE, KeyState::Down);

        input.BindCommand(kHighScoreAxis, KeyState::Down, std::make_unique<NameEntryAxisCommand>(m_nameEntry));
        input.BindCommand(SDL_SCANCODE_SPACE, KeyState::Down,
            std::make_unique<NameEntryConfirmCommand>(m_nameEntry, score, [this]() { FinishNameEntry(); }));

        m_nameEntryCtrlID = input.AddController();
        if (m_nameEntryCtrlID != UINT32_MAX)
        {
            input.BindCommand(m_nameEntryCtrlID, ControllerButton::DpadLeft, KeyState::Down,
                std::make_unique<NameEntryButtonCommand>(m_nameEntry, NameEntryButtonCommand::Action::SlotLeft));
            input.BindCommand(m_nameEntryCtrlID, ControllerButton::DpadRight, KeyState::Down,
                std::make_unique<NameEntryButtonCommand>(m_nameEntry, NameEntryButtonCommand::Action::SlotRight));
            input.BindCommand(m_nameEntryCtrlID, ControllerButton::DpadUp, KeyState::Down,
                std::make_unique<NameEntryButtonCommand>(m_nameEntry, NameEntryButtonCommand::Action::LetterUp));
            input.BindCommand(m_nameEntryCtrlID, ControllerButton::DpadDown, KeyState::Down,
                std::make_unique<NameEntryButtonCommand>(m_nameEntry, NameEntryButtonCommand::Action::LetterDown));
            input.BindCommand(m_nameEntryCtrlID, ControllerButton::ButtonA, KeyState::Down,
                std::make_unique<NameEntryConfirmCommand>(m_nameEntry, score, [this]() { FinishNameEntry(); }));
        }
    }

    void HighScoreState::FinishNameEntry()
    {
        for (auto* go : m_nameEntryObjects)
            go->m_destroy = true;
        m_nameEntryObjects.clear();
        m_nameEntry = nullptr;

        for (auto* label : m_labels)
            label->m_destroy = true;
        m_labels.clear();
        RebuildScoreList();

        auto& input = InputManager::GetInstance();
        input.UnbindCommand(kHighScoreAxis, KeyState::Down);
        input.UnbindCommand(SDL_SCANCODE_SPACE, KeyState::Down);

        if (m_nameEntryCtrlID != UINT32_MAX)
        {
            input.UnbindCommand(m_nameEntryCtrlID, ControllerButton::DpadLeft, KeyState::Down);
            input.UnbindCommand(m_nameEntryCtrlID, ControllerButton::DpadRight, KeyState::Down);
            input.UnbindCommand(m_nameEntryCtrlID, ControllerButton::DpadUp, KeyState::Down);
            input.UnbindCommand(m_nameEntryCtrlID, ControllerButton::DpadDown, KeyState::Down);
            input.UnbindCommand(m_nameEntryCtrlID, ControllerButton::ButtonA, KeyState::Down);
            m_nameEntryCtrlID = UINT32_MAX;
        }

        for (auto& fn : m_binding.onBind)
            fn();
    }

}