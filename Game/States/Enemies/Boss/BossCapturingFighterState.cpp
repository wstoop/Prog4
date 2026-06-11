// States/BossStates/BossCapturingFighterState.cpp
#include "BossCapturingFighterState.h"
#include "../InFormationState.h"
#include "../EnemyBrainComponent.h"
#include "Components/TransformComponent.h"
#include "Components/EnemyFormationSlotComponent.h"
//#include "Components/TractorBeamComponent.h"
#include "GameObject.h"
#include "TimeManager.h"

// We fire a scene-level event so the rest of the game (lives, spawning the
// captured-fighter enemy) can react without the state knowing about them.
#include "EventManager.h"   // your existing event bus
#include "Components/GameEvents.h"      // define FIGHTER_CAPTURED there

class TractorBeamComponent;   // TEMP
void dae::BossCapturingFighterState::OnEnter(EnemyBrainComponent& /*brain*/)
{
    m_elapsed = 0.f;
    m_docked = false;

    // 1. Notify the rest of the game: player loses a life, fighter turns red,
    //    etc.  All those systems listen for this event independently.
    EventManager::GetInstance().SendEvent(EVENT_FIGHTER_CAPTURED);
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

        // 2. Tell the beam component to finish and spawn the captured fighter
        //    as a new enemy next to the Boss in formation.
        auto* owner = brain.GetOwner();
        //if (auto* beam = owner->GetComponent<TractorBeamComponent>())
            //beam->FinalizeCapturedFighter();

        // 3. Boss reattaches to its formation slot and idles
        auto* transform = owner->GetComponent<TransformComponent>();

        auto* formationParent = brain.GetFormationParent();
        if (formationParent)
            transform->SetParent(formationParent, false);

        if (auto* slot = owner->GetComponent<EnemyFormationSlotComponent>())
        {
            transform->SetLocalPosition(slot->GetSlotLocalPos());
            slot->Activate();
        }

        transform->SetRotation(0.f, 0.f, 0.f);
        brain.TransitionTo(std::make_unique<InFormationState>());
    }
}

void dae::BossCapturingFighterState::OnExit(EnemyBrainComponent&) {}