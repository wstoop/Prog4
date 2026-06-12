// States/BossStates/BossCapturingFighterState.cpp
#include "BossCapturingFighterState.h"
#include "../InFormationState.h"
#include "../EnemyBrainComponent.h"
#include "Components/TransformComponent.h"
#include "Components/EnemyFormationSlotComponent.h"
#include "GameObject.h"
#include "TimeManager.h"

// We fire a scene-level event so the rest of the game (lives, spawning the
// captured-fighter enemy) can react without the state knowing about them.
#include "EventManager.h"   // your existing event bus
#include "Components/GameEvents.h"      // define FIGHTER_CAPTURED there

void dae::BossCapturingFighterState::OnEnter(EnemyBrainComponent& brain)
{
    m_elapsed = 0.f;
    m_docked = false;

    // 1. Notify the rest of the game: player loses a life, the capturing
    //    boss gets its evil escort ship, etc. All those systems listen for
    //    this event independently.
    EventManager::GetInstance().SendEvent(
        std::make_unique<DataEvent<CaptureEvent>>(EVENT_FIGHTER_CAPTURED, CaptureEvent{ brain.GetOwner(), m_capturedPlayerPos }));
}

void dae::BossCapturingFighterState::Update(EnemyBrainComponent& brain)
{
    if (m_docked) return;

    m_elapsed += dae::TimeManager::GetInstance().GetDeltaTime();

    // The TractorBeamComponent is responsible for animating the fighter moving
    // upward while this state is active (it polls its own elapsed time).
    // We just wait for the pull to finish.

    if (m_elapsed >= m_pullDuration)
    {
        m_docked = true;

        // 2. Boss returns to its formation slot via a top-entry loop
        brain.ReturnToFormationViaEntry();
    }
}

void dae::BossCapturingFighterState::OnExit(EnemyBrainComponent&) {}