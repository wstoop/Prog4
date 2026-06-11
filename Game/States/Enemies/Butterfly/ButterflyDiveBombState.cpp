// States/ButterflyStates/ButterflyDiveBombState.cpp
#include "ButterflyDiveBombState.h"
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
#include <cmath>
#include <glm/glm.hpp>

namespace {
    bool RollSwoop()
    {
        static std::mt19937 rng{ std::random_device{}() };
        return std::bernoulli_distribution{ 0.5 }(rng);
    }
}

void dae::ButterflyDiveBombState::OnEnter(EnemyBrainComponent& brain)
{
    brain.SetScoreMultiplier(2);
    m_doSwoop = RollSwoop();
    m_detached = false;
    m_currentWaypointIdx = 0;
    m_speed = 280.f;

    m_shootTimers = { 0.2f, 0.7f, 1.3f };

    auto* owner = brain.GetOwner();
    auto* transform = owner->GetComponent<TransformComponent>();
    glm::vec3 startPos = transform->GetWorldPosition();

    const float sw = static_cast<float>(GameInfo::GetInstance().GetGameWidth());
    const float sh = static_cast<float>(GameInfo::GetInstance().GetScreenHeight());
    float centerDirection = (startPos.x < sw * 0.5f) ? 1.f : -1.f;

    // --- GENERATE ERRATIC JAGGED WAYPOINTS ---
    m_waypoints.clear();

    // Jag 1: Sharp dash diagonally down and completely across the center line
    m_waypoints.push_back(startPos + glm::vec3(centerDirection * (sw * 0.55f), sh * 0.25f, 0.f));

    // Jag 2: Quick, erratic twitch back outward horizontally to confuse the player
    m_waypoints.push_back(startPos + glm::vec3(centerDirection * (sw * 0.20f), sh * 0.45f, 0.f));

    // Jag 3: Hard diagonal cut back inward towards the bottom middle
    m_waypoints.push_back(startPos + glm::vec3(centerDirection * (sw * 0.45f), sh * 0.70f, 0.f));

    // Jag 4: Exit point deep past the bottom layout boundary
    m_waypoints.push_back(glm::vec3(startPos.x, sh + 80.f, 0.f));

    // Detach context for absolute tracking
    if (transform->GetParent())
    {
        transform->SetParent(nullptr, false);
        transform->SetLocalPosition(startPos);
        m_detached = true;
    }

    if (auto* slot = owner->GetComponent<EnemyFormationSlotComponent>())
        slot->Deactivate();
}

void dae::ButterflyDiveBombState::Update(EnemyBrainComponent& brain)
{
    if (m_currentWaypointIdx >= m_waypoints.size()) return;

    auto* owner = brain.GetOwner();
	auto* data = owner->GetComponent<EnemyDataComponent>();
    auto* t = owner->GetComponent<TransformComponent>();
    if (data && !data->IsPlayerAlive())
    {
        auto* formationParent = brain.GetFormationParent();
        if (formationParent)
            t->SetParent(formationParent, false);

        if (auto* slot = owner->GetComponent<EnemyFormationSlotComponent>())
        {
            t->SetLocalPosition(slot->GetSlotLocalPos());
            slot->Activate();
        }

        t->SetRotation(0.f, 0.f, 0.f);
        brain.TransitionTo(std::make_unique<InFormationState>());
        return;
    }


    glm::vec3 currentPos = t->GetLocalPosition();
    glm::vec3 targetPos = m_waypoints[m_currentWaypointIdx];

    // Calculate heading vector
    glm::vec3 directionVec = targetPos - currentPos;
    float distance = glm::length(directionVec);

    float dt = dae::TimeManager::GetInstance().GetDeltaTime();
    float moveDist = m_speed * dt;

    if (moveDist >= distance)
    {
        // Snap to waypoint and target the next jagged turn
        t->SetLocalPosition(targetPos);
        m_currentWaypointIdx++;
    }
    else
    {
        // Move along the linear segment path
        glm::vec3 velocity = (directionVec / distance) * m_speed;
        t->SetLocalPosition(currentPos + velocity * dt);

        // Update the sharp rotation targeting the line segment direction
        float angle = std::atan2(velocity.y, velocity.x) - (3.14159265f * 0.5f);
        t->SetRotation(0.f, 0.f, glm::degrees(angle) + 180.f);
    }

    // --- Shooting Logic ---
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
    if (m_currentWaypointIdx >= m_waypoints.size())
    {

        auto* formationParent = brain.GetFormationParent();
        if (formationParent)
            t->SetParent(formationParent, false);

        if (auto* slot = owner->GetComponent<EnemyFormationSlotComponent>())
        {
            t->SetLocalPosition(slot->GetSlotLocalPos());
            slot->Activate();
        }

        t->SetRotation(0.f, 0.f, 0.f);
        brain.TransitionTo(std::make_unique<InFormationState>());
    }
}

void dae::ButterflyDiveBombState::OnExit(EnemyBrainComponent&) {}