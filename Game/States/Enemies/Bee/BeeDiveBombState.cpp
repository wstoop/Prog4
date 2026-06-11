// States/BeeStates/BeeDiveBombState.cpp
#include "BeeDiveBombState.h"
#include "BeeSwoopState.h"
#include "../InFormationState.h"
#include "../EnemyBrainComponent.h"
#include "Components/TransformComponent.h"
#include "Components/EnemyFormationSlotComponent.h"
#include "Components/ShootComponent.h"
#include "Components/EnemyDataComponent.h"
#include "GameInfo.h"
#include "GameObject.h"
#include "TimeManager.h"
#include <random>
#include <glm/glm.hpp>

namespace {
    bool RollSwoop()
    {
        static std::mt19937 rng{ std::random_device{}() };
        return std::bernoulli_distribution{ 0.5 }(rng);
    }
}


glm::vec3 dae::BeeDiveBombState::EvalBezier(float t) const
{
    float u = 1.f - t;
    return u * u * u * m_p0
        + 3.f * u * u * t * m_p1
        + 3.f * u * t * t * m_p2
        + t * t * t * m_p3;
}

glm::vec3 dae::BeeDiveBombState::EvalBezierTangent(float t) const
{
    float u = 1.f - t;
    return 3.f * (u * u * (m_p1 - m_p0) + 2.f * u * t * (m_p2 - m_p1) + t * t * (m_p3 - m_p2));
}

void dae::BeeDiveBombState::OnEnter(EnemyBrainComponent& brain)
{
    brain.SetScoreMultiplier(2);
    m_doSwoop = RollSwoop();
    m_detached = false;
    m_elapsed = 0.f;
    m_duration = 3.2f;

    m_shootTimers = { 0.3f, 0.9f, 1.6f };

    auto* owner = brain.GetOwner();
    auto* transform = owner->GetComponent<TransformComponent>();

    m_p0 = transform->GetWorldPosition();

    const float sw = static_cast<float>(GameInfo::GetInstance().GetGameWidth());
    const float sh = static_cast<float>(GameInfo::GetInstance().GetScreenHeight());

    float centerDirection = (m_p0.x < sw * 0.5f) ? 1.f : -1.f;

    m_p1 = m_p0 + glm::vec3(centerDirection * (sw * 0.65f), sh * 0.35f, 0.f);
    m_p2 = m_p0 + glm::vec3(centerDirection * -(sw * 0.35f), sh * 0.65f, 0.f);
    m_p3 = glm::vec3(m_p0.x + (centerDirection * -(sw * 0.15f)), sh + 80.f, 0.f);

    if (transform->GetParent())
    {
        transform->SetParent(nullptr, false);
        transform->SetLocalPosition(m_p0);
        m_detached = true;
    }

    if (auto* slot = owner->GetComponent<EnemyFormationSlotComponent>())
        slot->Deactivate();
}

void dae::BeeDiveBombState::Update(EnemyBrainComponent& brain)
{
    auto* owner = brain.GetOwner();
    auto* data = owner->GetComponent<EnemyDataComponent>();

    if (data && !data->IsPlayerAlive())
    {
        auto* t = owner->GetComponent<TransformComponent>();
        // Cleanly position them to start an immediate swoop back home
        t->SetLocalPosition(t->GetWorldPosition());
        brain.TransitionTo(std::make_unique<BeeSwoopState>());
        return;
    }

    auto* t = owner->GetComponent<TransformComponent>();

    m_elapsed += dae::TimeManager::GetInstance().GetDeltaTime();
    float progress = m_elapsed / m_duration;
    if (progress > 1.f) progress = 1.f;

    glm::vec3 pos = EvalBezier(progress);
    t->SetLocalPosition(pos);

    if (progress < 1.f)
    {
        glm::vec3 tangent = EvalBezierTangent(progress);

        float angle = std::atan2(tangent.y, tangent.x) - (3.14159265f * 0.5f);
        t->SetRotation(0.f, 0.f, glm::degrees(angle) + 180);
    }

    // --- Shooting Logic ---
    if (!m_shootTimers.empty())
    {
        m_shootTimers.front() -= dae::TimeManager::GetInstance().GetDeltaTime();
        if (m_shootTimers.front() <= 0.f)
        {
            if (auto* shooter = owner->GetComponent<ShootComponent>())
                shooter->Shoot(0.f, 1.f);
            m_shootTimers.erase(m_shootTimers.begin());
        }
    }

    if (progress >= 1.f)
    {
        if (m_doSwoop && data->IsPlayerAlive())
        {
            pos.y = GameInfo::GetInstance().GetScreenHeight() + 80.f;
            t->SetLocalPosition(pos);

            brain.TransitionTo(std::make_unique<BeeSwoopState>());
        }
        else
        {
            auto* formationParent = brain.GetFormationParent();
            if (formationParent)
                t->SetParent(formationParent, false);
            auto* slot = owner->GetComponent<EnemyFormationSlotComponent>();

            if (slot)
            {
                t->SetLocalPosition(slot->GetSlotLocalPos());
                slot->Activate();
            }

            t->SetRotation(0.f, 0.f, 0.f);
            brain.TransitionTo(std::make_unique<InFormationState>());
        }
    }
}


void dae::BeeDiveBombState::OnExit(EnemyBrainComponent&) {}