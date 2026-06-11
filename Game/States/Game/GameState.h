#pragma once
#include "SceneBuilder.h"
#include "SceneManager.h"
#include "../../Components/GameEvents.h"

namespace dae
{
    class GameStateManager;

    class GameState
    {
    public:
        explicit GameState(GameStateManager& manager) : m_manager(manager) {}
        virtual ~GameState() = default;

        virtual void OnEnter(SceneInputBinding& previous) = 0;
        virtual void OnExit() {}

        SceneInputBinding& GetBinding() { return m_binding; }

    protected:
        GameStateManager& m_manager;
        SceneInputBinding  m_binding{};
    };

    class MenuState : public GameState
    {
    public:
        explicit MenuState(GameStateManager& m);
        void OnEnter(SceneInputBinding& previous) override;
        void OnExit() override;
    };

    class SinglePlayerState : public GameState
    {
    public:
        explicit SinglePlayerState(GameStateManager& m);
        void OnEnter(SceneInputBinding& previous) override;
    };

    class CoOpState : public GameState
    {
    public:
        explicit CoOpState(GameStateManager& m);
        void OnEnter(SceneInputBinding& previous) override;
    };

    class VersusState : public GameState
    {
    public:
        explicit VersusState(GameStateManager& m);
        void OnEnter(SceneInputBinding& previous) override;
    };

    class EndScreenState : public GameState
    {
    public:
        explicit EndScreenState(GameStateManager& m);
        void OnEnter(SceneInputBinding& previous) override;
    };

    class HighScoreState : public GameState
    {
    public:
        explicit HighScoreState(GameStateManager& m);
        void OnEnter(SceneInputBinding& previous) override;
    };
}