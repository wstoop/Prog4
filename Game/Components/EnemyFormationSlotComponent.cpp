#include "EnemyFormationSlotComponent.h"
#include "FormationComponent.h"
#include "Components/TransformComponent.h"
#include "GameObject.h"

dae::EnemyFormationSlotComponent::EnemyFormationSlotComponent(
    GameObject* owner,
    FormationComponent* formation,
    const glm::vec3& slotLocalPos,
    const glm::vec3& formationCenter)
    : Component(owner)
    , m_formation(formation)
    , m_slotLocalPos(slotLocalPos)
    , m_formationCenter(formationCenter)
{}

void dae::EnemyFormationSlotComponent::Activate()
{
    if (m_active) return;
    m_active = true;
    m_formation->NotifyDocked();
}

void dae::EnemyFormationSlotComponent::Deactivate()
{
    if (!m_active) return;
    m_active = false;
    m_formation->NotifyUndocked();   // ← you'll add this to FormationComponent
}

void dae::EnemyFormationSlotComponent::NotifyIfDied()
{
    if (m_notifiedDeath) return;
    m_notifiedDeath = true;
    m_formation->NotifyDied();
}

glm::vec3 dae::EnemyFormationSlotComponent::ComputeSway() const
{
    return m_formation->ComputeSwayOffset(m_slotLocalPos, m_formationCenter);
}

glm::vec3 dae::EnemyFormationSlotComponent::GetSlotWorldPos() const
{
    // Formation parent world pos + this slot's local offset (+ current sway)
    auto* formationTransform = m_formation->GetOwner()->GetComponent<TransformComponent>();
    return formationTransform->GetWorldPosition() + m_slotLocalPos;
    // Note: intentionally excludes sway — we want the stable slot centre,
    // not a moving target for the swoop to chase.
}

dae::EnemyFormationSlotComponent::~EnemyFormationSlotComponent()
{
    NotifyIfDied();
}