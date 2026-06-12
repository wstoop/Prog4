// States/BossStates/BossDiveBombState.cpp
#include "BossDiveBombState.h"
#include "../InFormationState.h"
#include "../EnemyBrainComponent.h"
#include "Components/TransformComponent.h"
#include "Components/EnemyFormationSlotComponent.h"
#include "Components/ShootComponent.h"
#include "GameInfo.h"
#include "GameObject.h"
#include "TimeManager.h"
#include <cmath>
#include <glm/glm.hpp>

// ---- Bézier helpers ----

glm::vec3 dae::BossDiveBombState::EvalBezier(float t) const
{
    float u = 1.f - t;
    return u * u * u * m_p0
        + 3.f * u * u * t * m_p1
        + 3.f * u * t * t * m_p2
        + t * t * t * m_p3;
}

glm::vec3 dae::BossDiveBombState::EvalBezierTangent(float t) const
{
    float u = 1.f - t;
    return 3.f * (u * u * (m_p1 - m_p0) + 2.f * u * t * (m_p2 - m_p1) + t * t * (m_p3 - m_p2));
}

// ---- State lifecycle ----

void dae::BossDiveBombState::OnEnter(EnemyBrainComponent& brain)
{
    brain.SetScoreMultiplier(2);
    m_elapsed = 0.f;
    m_duration = 3.6f;
    m_detached = false;
    m_shootTimers = { 0.4f, 1.1f, 1.9f };

    auto* owner = brain.GetOwner();
    auto* transform = owner->GetComponent<TransformComponent>();

    m_p0 = transform->GetWorldPosition();

    const float sw = static_cast<float>(GameInfo::GetInstance().GetGameWidth());
    const float sh = static_cast<float>(GameInfo::GetInstance().GetScreenHeight());

    // Same S-curve logic as BeeDiveBombState — sweeping arc toward center then out
    float dir = (m_p0.x < sw * 0.5f) ? 1.f : -1.f;

    m_p1 = m_p0 + glm::vec3(dir * (sw * 0.65f), sh * 0.35f, 0.f);
    m_p2 = m_p0 + glm::vec3(-dir * (sw * 0.35f), sh * 0.65f, 0.f);
    m_p3 = glm::vec3(m_p0.x - dir * (sw * 0.15f), sh + 80.f, 0.f);

    // Detach from formation so we can move in world space
    if (transform->GetParent())
    {
        transform->SetParent(nullptr, false);
        transform->SetLocalPosition(m_p0);
        m_detached = true;
    }

    if (auto* slot = owner->GetComponent<EnemyFormationSlotComponent>())
        slot->Deactivate();
}

void dae::BossDiveBombState::Update(EnemyBrainComponent& brain)
{
    float dt = dae::TimeManager::GetInstance().GetDeltaTime();
    m_elapsed += dt;

    float progress = std::min(m_elapsed / m_duration, 1.f);

    auto* owner = brain.GetOwner();
    auto* transform = owner->GetComponent<TransformComponent>();

    glm::vec3 pos = EvalBezier(progress);
    transform->SetLocalPosition(pos);

    // Keep the sprite pointing along the curve
    if (progress < 1.f)
    {
        glm::vec3 tangent = EvalBezierTangent(progress);
        float angle = std::atan2(tangent.y, tangent.x) - (3.14159265f * 0.5f);
        transform->SetRotation(0.f, 0.f, glm::degrees(angle) + 180.f);
    }

    // Scheduled shots
    if (!m_shootTimers.empty())
    {
        m_shootTimers.front() -= dt;
        if (m_shootTimers.front() <= 0.f)
        {
            if (auto* shooter = owner->GetComponent<ShootComponent>())
                shooter->Shoot(0.f, 1.f);
            m_shootTimers.erase(m_shootTimers.begin());
        }
    }

    // Reached the bottom — return to formation via a top-entry loop
    if (progress >= 1.f)
    {
        brain.ReturnToFormationViaEntry();
    }
}

void dae::BossDiveBombState::OnExit(EnemyBrainComponent&) {}