#include "EnemyFormationSlotComponent.h"
#include "TransformComponent.h"
#include "FormationComponent.h"
#include "../GameObject.h"

dae::EnemyFormationSlotComponent::EnemyFormationSlotComponent(
    GameObject* owner,
    FormationComponent* formation,
    const glm::vec3& slotLocalPos,
    const glm::vec3& formationCenter)
    : Component(owner)
    , m_formation(formation)
    , m_slotLocalPos(slotLocalPos)
    , m_formationCenter(formationCenter)
{
    m_transform = owner->GetComponent<TransformComponent>();
}

void dae::EnemyFormationSlotComponent::Update()
{
    if (!m_active) return;
    if (GetOwner()->m_destroy) return;

    glm::vec3 sway = m_formation->ComputeSwayOffset(m_slotLocalPos, m_formationCenter);
    m_transform->SetLocalPosition(m_slotLocalPos + sway);
}

void dae::EnemyFormationSlotComponent::Activate()
{
    m_active = true;
    m_formation->NotifyDocked();
}

void dae::EnemyFormationSlotComponent::NotifyIfDied()
{
    if (m_notifiedDeath) return;
    m_notifiedDeath = true;
    m_formation->NotifyDied();
}

dae::EnemyFormationSlotComponent::~EnemyFormationSlotComponent()
{
    NotifyIfDied();
}