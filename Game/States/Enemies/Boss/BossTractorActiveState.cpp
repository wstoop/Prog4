// States/BossStates/BossTractorActiveState.cpp
#include "BossTractorActiveState.h"
#include "BossCapturingFighterState.h"
#include "../InFormationState.h"
#include "../EnemyBrainComponent.h"
#include "Components/TransformComponent.h"
#include "Components/EnemyFormationSlotComponent.h"
//#include "Components/TractorBeamComponent.h"   // you'll create this
#include "GameObject.h"
#include "TimeManager.h"

class TractorBeamComponent;   // TEMP
void dae::BossTractorActiveState::OnEnter(EnemyBrainComponent& /*brain*/)
{
    m_elapsed = 0.f;
    m_captured = false;

    // Tell the visual component to open the beam and register this state
    // so it can call NotifyCaptured() when the player enters the cone.
    //auto* owner = brain.GetOwner();
    //if (auto* beam = owner->GetComponent<TractorBeamComponent>())
        //beam->Activate(this);
}

void dae::BossTractorActiveState::Update(EnemyBrainComponent& brain)
{
    m_elapsed += dae::TimeManager::GetInstance().GetDeltaTime();

    if (m_captured)
    {
        brain.TransitionTo(std::make_unique<BossCapturingFighterState>());
        return;
    }

    if (m_elapsed >= m_beamDuration)
        ReturnToFormation(brain);
}

void dae::BossTractorActiveState::OnExit(EnemyBrainComponent& /*brain*/)
{
    // Close the beam regardless of how we exit
    //auto* owner = brain.GetOwner();
    //if (auto* beam = owner->GetComponent<TractorBeamComponent>())
        //beam->Deactivate();
}

void dae::BossTractorActiveState::ReturnToFormation(EnemyBrainComponent& brain)
{
    auto* owner = brain.GetOwner();
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