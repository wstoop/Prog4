#include "EnemyBrainComponent.h"
#include "EnteringState.h"
#include "Components/AnimationComponent.h"
#include "Components/EnemyFormationSlotComponent.h"
#include "Components/TransformComponent.h"
#include "GameInfo.h"
#include "GameObject.h"

dae::EnemyBrainComponent::EnemyBrainComponent(GameObject* owner)
    : Component(owner)
{
    // Don't call OnEnter yet — SetEntryConfig() hasn't been called.
    // The first Update() will initialise the state.
    m_currentState = std::make_unique<EnteringState>();
}

void dae::EnemyBrainComponent::Update()
{
    if (!m_currentState) return;

    // Deferred OnEnter: wait until SetEntryConfig has been called (delay > 0 is a
    // reliable proxy; even delay==0 is fine because we set m_entryConfigSet explicitly).
    if (!m_entryConfigSet) return;

    if (!m_entered)
    {
        m_currentState->OnEnter(*this);
        m_entered = true;
    }

    m_currentState->Update(*this);
}

void dae::EnemyBrainComponent::TransitionTo(std::unique_ptr<IEnemyState> next)
{
    if (m_currentState) m_currentState->OnExit(*this);
    m_currentState = std::move(next);
    if (m_currentState) m_currentState->OnEnter(*this);
}

void dae::EnemyBrainComponent::ReturnToFormationViaEntry()
{
    auto* owner = GetOwner();
    auto* transform = owner->GetComponent<TransformComponent>();
    transform->SetRotation(0.f, 0.f, 0.f);

    glm::vec3 target = transform->GetWorldPosition();
    if (auto* slot = owner->GetComponent<EnemyFormationSlotComponent>())
        target = slot->GetSlotWorldPos();

    const float sw = static_cast<float>(GameInfo::GetInstance().GetGameWidth());
    const bool fromLeft = transform->GetWorldPosition().x < sw * 0.5f;

    SetEntryConfig(target, 2.f, 0.f, fromLeft, m_formationParent);
    TransitionTo(std::make_unique<EnteringState>());
}

void dae::EnemyBrainComponent::MarkHit()
{
    if (m_isHit) return;
    m_isHit = true;
    GetOwner()->GetComponent<AnimationComponent>()->SetRow(2);
}