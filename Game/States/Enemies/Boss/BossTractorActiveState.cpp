// States/BossStates/BossTractorActiveState.cpp
#include "BossTractorActiveState.h"
#include "BossCapturingFighterState.h"
#include "../InFormationState.h"
#include "../EnemyBrainComponent.h"
#include "Components/TransformComponent.h"
#include "Components/EnemyFormationSlotComponent.h"
#include "Components/TractorBeamComponent.h"
#include "GameObject.h"
#include "TimeManager.h"

void dae::BossTractorActiveState::OnEnter(EnemyBrainComponent& brain)
{
    m_elapsed = 0.f;
    m_captured = false;


    auto* owner = brain.GetOwner();
    if (auto* beam = owner->GetComponent<TractorBeamComponent>())
        beam->Activate(this);
}

void dae::BossTractorActiveState::Update(EnemyBrainComponent& brain)
{
    m_elapsed += dae::TimeManager::GetInstance().GetDeltaTime();

    if (m_captured)
    {
        brain.TransitionTo(std::make_unique<BossCapturingFighterState>(m_capturedPos));
        return;
    }

    if (m_elapsed >= m_beamDuration)
        ReturnToFormation(brain);
}

void dae::BossTractorActiveState::OnExit(EnemyBrainComponent& brain)
{
    // Close the beam regardless of how we exit
    auto* owner = brain.GetOwner();
    if (auto* beam = owner->GetComponent<TractorBeamComponent>())
        beam->Deactivate();
}

void dae::BossTractorActiveState::ReturnToFormation(EnemyBrainComponent& brain)
{
    brain.ReturnToFormationViaEntry();
}