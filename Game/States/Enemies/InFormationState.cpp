#include "InFormationState.h"
#include "EnemyBrainComponent.h"
#include "Components/EnemyDataComponent.h"
#include "Components/EnemyFormationSlotComponent.h"
#include "Components/TransformComponent.h"
#include "GameObject.h"
#include "TimeManager.h"
#include <random>

static float RandomDiveDelay()
{
    static std::mt19937 rng{ std::random_device{}() };
    std::uniform_real_distribution<float> dist(2.f, 8.f);
    return dist(rng);
}

void dae::InFormationState::OnEnter(EnemyBrainComponent& brain)
{
    brain.SetScoreMultiplier(1);
    m_diveTimer = RandomDiveDelay();
}

void dae::InFormationState::Update(EnemyBrainComponent& brain)
{
    auto* owner = brain.GetOwner();
    auto* slot = owner->GetComponent<EnemyFormationSlotComponent>();

    if (slot)
    {
        auto* transform = owner->GetComponent<TransformComponent>();
        transform->SetLocalPosition(slot->GetSlotLocalPos() + slot->ComputeSway());
    }

    m_diveTimer -= TimeManager::GetInstance().GetDeltaTime();
    if (m_diveTimer <= 0.f)
    {
        auto* data = owner->GetComponent<EnemyDataComponent>();
        if (data && data->GetCombatStateFactory() && data->IsPlayerAlive())
            brain.TransitionTo(data->GetCombatStateFactory()());
        else
            m_diveTimer = RandomDiveDelay();
    }
}