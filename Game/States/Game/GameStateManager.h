#pragma once
#include <memory>
#include "GameState.h"
#include "EventManager.h"
#include "GameStateData.h"
#include "../../Components/GameEvents.h"

namespace dae
{
    class GameStateManager : public EventHandler
    {
    public:
        GameStateManager()
        {
            m_menuState = std::make_unique<MenuState>(*this);
            m_singleState = std::make_unique<SinglePlayerState>(*this);
            m_coopState = std::make_unique<CoOpState>(*this);
            m_versusState = std::make_unique<VersusState>(*this);
            m_endState = std::make_unique<EndScreenState>(*this);
            m_highScoreState = std::make_unique<HighScoreState>(*this);

            auto& em = EventManager::GetInstance();
            em.AttachEvent(EVENT_LOAD_START, this);
            em.AttachEvent(EVENT_LOAD_SINGLE, this);
            em.AttachEvent(EVENT_LOAD_COOP, this);
            em.AttachEvent(EVENT_LOAD_VERSUS, this);
            em.AttachEvent(EVENT_LOAD_END, this);
            em.AttachEvent(EVENT_LOAD_HIGHSCORE, this);
            em.AttachEvent(EVENT_PLAYER_DAMAGED_ENEMY, this);
            em.AttachEvent(EVENT_PLAYER_SHOT, this);
            em.AttachEvent(DATEVENT_SCORE_CHANGED, this);
			em.AttachEvent(EVENT_GAME_OVER, this);

            m_currentState = m_menuState.get();
            m_currentState->GetBinding().Bind();
            SceneManager::GetInstance().SetActiveScene("Menu");
        }

        GameStateData& GetSharedData() { return m_sharedData; }
        void ResetSharedData() { m_sharedData = GameStateData{}; }
        void HandleEvent(const Event* pEvent) override
        {
            switch (pEvent->id)
            {
            case EVENT_LOAD_START:  SetState(m_menuState.get());   break;
            case EVENT_LOAD_SINGLE: SetState(m_singleState.get()); break;
            case EVENT_LOAD_COOP:   SetState(m_coopState.get());   break;
            case EVENT_LOAD_VERSUS: SetState(m_versusState.get()); break;
            case EVENT_LOAD_END:    SetState(m_endState.get());    break;
			case EVENT_GAME_OVER:   SetState(m_endState.get());    break;
            case EVENT_LOAD_HIGHSCORE: SetState(m_highScoreState.get()); break;
			case EVENT_ENEMY_DIED:  m_sharedData.ShotsHit++; break;
			case EVENT_PLAYER_SHOT: m_sharedData.ShotsFired++; break;
            }

            const auto* scoreEvent = dynamic_cast<const DataEvent<ScoreEvent>*>(pEvent);
            if (!scoreEvent) return;
            m_sharedData.Score += scoreEvent->data.points;
        }

    private:
        void SetState(GameState* newState)
        {
            if (newState == m_currentState) return;

            m_currentState->OnExit();
            newState->OnEnter(m_currentState->GetBinding());
            m_currentState = newState;
        }

        GameState* m_currentState{ nullptr };
        GameState* m_pPreviousGameplayState{ nullptr };

        GameStateData m_sharedData{};

        std::unique_ptr<GameState> m_menuState;
        std::unique_ptr<GameState> m_singleState;
        std::unique_ptr<GameState> m_coopState;
        std::unique_ptr<GameState> m_versusState;
        std::unique_ptr<GameState> m_endState;
        std::unique_ptr<GameState> m_pauseState;
		std::unique_ptr<GameState> m_highScoreState;
    };
}