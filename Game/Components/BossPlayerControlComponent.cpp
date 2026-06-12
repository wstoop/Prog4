#include "BossPlayerControlComponent.h"
#include "EnemyFormationSlotComponent.h"
#include "States/Enemies/EnemyBrainComponent.h"
#include "States/Enemies/Boss/BossDiveBombState.h"
#include "States/Enemies/Boss/BossTractorApproachState.h"
#include "GameObject.h"
#include "TimeManager.h"

dae::BossPlayerControlComponent::BossPlayerControlComponent(GameObject* owner)
    : Component(owner)
{
}

void dae::BossPlayerControlComponent::Update()
{
    const float dt = TimeManager::GetInstance().GetDeltaTime();

    if (m_diveCooldown > 0.f)
        m_diveCooldown -= dt;

    if (m_tractorCooldown > 0.f)
        m_tractorCooldown -= dt;
}

bool dae::BossPlayerControlComponent::CanAttack() const
{
    auto* slot = GetOwner()->GetComponent<EnemyFormationSlotComponent>();
    return slot && slot->IsActive();
}

void dae::BossPlayerControlComponent::TriggerDiveBomb()
{
    if (m_diveCooldown > 0.f || !CanAttack()) return;

    m_diveCooldown = k_diveCooldownTime;
    GetOwner()->GetComponent<EnemyBrainComponent>()->TransitionTo(std::make_unique<BossDiveBombState>());
}

void dae::BossPlayerControlComponent::TriggerTractorBeam()
{
    if (m_tractorCooldown > 0.f || !CanAttack()) return;

    m_tractorCooldown = k_tractorCooldownTime;
    GetOwner()->GetComponent<EnemyBrainComponent>()->TransitionTo(std::make_unique<BossTractorApproachState>());
}
