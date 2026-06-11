// States/BeeStates/BeeSwoopState.cpp
#include "BeeSwoopState.h"
#include "../InFormationState.h"
#include "../EnemyBrainComponent.h"
#include "Components/TransformComponent.h"
#include "Components/EnemyFormationSlotComponent.h"
#include "Components/ShootComponent.h"
#include "Components/EnemyDataComponent.h"
#include "GameObject.h"
#include "TimeManager.h"
#include "GameInfo.h"
#include <cmath>
#include <glm/glm.hpp>

glm::vec3 dae::BeeSwoopState::EvalBezier(float t) const
{
    float u = 1.f - t;
    return u * u * u * m_p0
        + 3.f * u * u * t * m_p1
        + 3.f * u * t * t * m_p2
        + t * t * t * m_p3;
}

glm::vec3 dae::BeeSwoopState::EvalBezierTangent(float t) const
{
    float u = 1.f - t;
    return 3.f * (u * u * (m_p1 - m_p0) + 2.f * u * t * (m_p2 - m_p1) + t * t * (m_p3 - m_p2));
}

void dae::BeeSwoopState::OnEnter(EnemyBrainComponent& brain)
{
    auto* owner = brain.GetOwner();
    auto* transform = owner->GetComponent<TransformComponent>();
    auto* slot = owner->GetComponent<EnemyFormationSlotComponent>();

    m_elapsed = 0.f;
    m_duration = 3.4f;
    m_shootTimers = { 0.4f, 1.2f, 2.1f };

    // Start at bottom boundary (assigned right before transitioning here)
    m_p0 = transform->GetWorldPosition();

    // End at its designated target formation slot
    m_p3 = slot ? slot->GetSlotWorldPos() : m_p0;

    const float sw = static_cast<float>(GameInfo::GetInstance().GetGameWidth());
    const float sh = static_cast<float>(GameInfo::GetInstance().GetScreenHeight());

    float centerDirection = (m_p0.x < sw * 0.5f) ? 1.f : -1.f;

    // --- REVERSED UPWARD S-CURVE ---
    // P1: Loops aggressively upward and across the screen center from the bottom
    m_p1 = m_p0 + glm::vec3(centerDirection * (sw * 0.65f), -(sh * 0.35f), 0.f);

    // P2: Curves outward to align horizontally with its slot position
    m_p2 = m_p3 + glm::vec3(centerDirection * -(sw * 0.25f), sh * 0.15f, 0.f);
}

void dae::BeeSwoopState::Update(EnemyBrainComponent& brain)
{
    float dt = TimeManager::GetInstance().GetDeltaTime();
    m_elapsed += dt;
    float t = m_elapsed / m_duration;

    auto* owner = brain.GetOwner();
    auto* transform = owner->GetComponent<TransformComponent>();
    auto* data = owner->GetComponent<EnemyDataComponent>();

    // --- Shooting Logic ---
    if (!m_shootTimers.empty() && data && data->IsPlayerAlive())
    {
        m_shootTimers.front() -= dt;
        if (m_shootTimers.front() <= 0.f)
        {
            if (auto* shooter = owner->GetComponent<ShootComponent>())
                shooter->Shoot(0.f, 1.f);
            m_shootTimers.erase(m_shootTimers.begin());
        }
    }

    if (t >= 1.f)
    {
        transform->SetLocalPosition(m_p3);

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
    else
    {
        glm::vec3 pos = EvalBezier(t);
        transform->SetLocalPosition(pos);

        glm::vec3 tangent = EvalBezierTangent(t);
        float angle = std::atan2(tangent.y, tangent.x) - (3.14159265f * 0.5f);
        transform->SetRotation(0.f, 0.f, glm::degrees(angle) + 180.f);
    }
}

void dae::BeeSwoopState::OnExit(EnemyBrainComponent&) {}