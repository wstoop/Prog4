// States/BossStates/BossTractorApproachState.cpp
#include "BossTractorApproachState.h"
#include "BossTractorActiveState.h"
#include "../EnemyBrainComponent.h"
#include "Components/TransformComponent.h"
#include "Components/EnemyFormationSlotComponent.h"
#include "GameInfo.h"
#include "GameObject.h"
#include "TimeManager.h"
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

void dae::BossTractorApproachState::OnEnter(EnemyBrainComponent& brain)
{
    m_elapsed = 0.f;
    m_detached = false;

    auto* owner = brain.GetOwner();
    auto* transform = owner->GetComponent<TransformComponent>();

    const float sw = static_cast<float>(GameInfo::GetInstance().GetGameWidth());
    const float sh = static_cast<float>(GameInfo::GetInstance().GetScreenHeight());

    glm::vec3 startPos = transform->GetWorldPosition();

    // Hover at horizontal center, roughly 45 % down the screen
    m_destination = glm::vec3(sw * 0.5f, sh * 0.45f, 0.f);

    BuildApproachPath(startPos, m_destination);

    // Detach so we can roam freely in world space
    if (transform->GetParent())
    {
        transform->SetParent(nullptr, false);
        transform->SetLocalPosition(startPos);
        m_detached = true;
    }

    if (auto* slot = owner->GetComponent<EnemyFormationSlotComponent>())
        slot->Deactivate();
}

void dae::BossTractorApproachState::Update(EnemyBrainComponent& brain)
{
    float dt = dae::TimeManager::GetInstance().GetDeltaTime();
    m_elapsed += dt;

    float totalT = std::min(m_elapsed / m_duration, 1.f);

    auto* owner = brain.GetOwner();
    auto* transform = owner->GetComponent<TransformComponent>();

    // Find active segment (same logic as EnteringState)
    const Segment* activeSeg = nullptr;
    float localT = 0.f;

    for (const auto& seg : m_segments)
    {
        if (totalT <= seg.timeEnd)
        {
            float range = seg.timeEnd - seg.timeStart;
            localT = range > 0.f ? (totalT - seg.timeStart) / range : 0.f;
            activeSeg = &seg;
            break;
        }
    }
    if (!activeSeg)
    {
        activeSeg = &m_segments.back();
        localT = 1.f;
    }

    glm::vec3 pos{};
    float angle = 0.f;

    if (activeSeg->type == SegmentType::Line)
    {
        pos = glm::mix(activeSeg->p0, activeSeg->p3, localT);
        glm::vec3 nextPos = glm::mix(activeSeg->p0, activeSeg->p3, std::min(localT + 0.001f, 1.f));
        glm::vec3 dir = nextPos - pos;
        if (glm::length(dir) > 0.0001f) dir = glm::normalize(dir);
        angle = std::atan2(dir.y, dir.x);
    }
    else
    {
        float currentAngle = activeSeg->startAngle + activeSeg->sweepAngle * localT;
        pos = activeSeg->center + glm::vec3(
            std::cos(currentAngle) * activeSeg->radius,
            std::sin(currentAngle) * activeSeg->radius,
            0.f);
        glm::vec2 tangent(-std::sin(currentAngle), std::cos(currentAngle));
        if (activeSeg->sweepAngle < 0.f) tangent = -tangent;
        angle = std::atan2(tangent.y, tangent.x);
    }

    transform->SetLocalPosition(pos);
    transform->SetRotation(0.f, 0.f, glm::degrees(angle) + 90.f);

    if (totalT >= 1.f)
    {
        transform->SetLocalPosition(m_destination);
        transform->SetRotation(0.f, 0.f, 0.f);
        brain.TransitionTo(std::make_unique<BossTractorActiveState>());
    }
}

void dae::BossTractorApproachState::OnExit(EnemyBrainComponent&) {}

// ---- Path: top-loop then straight slide down to mid-screen ----

void dae::BossTractorApproachState::BuildApproachPath(
    const glm::vec3& startPos,
    const glm::vec3& midScreenPos)
{
    m_segments.clear();

    const float sw = static_cast<float>(GameInfo::GetInstance().GetGameWidth());
    const float loopRadius = 55.f;

    // Determine which side the Boss is coming from
    bool fromLeft = (startPos.x < sw * 0.5f);
    float dir = fromLeft ? 1.f : -1.f;

    // --- Segment 1: Short line from formation slot toward the loop entry ---
    glm::vec3 loopEntry = startPos + glm::vec3(dir * loopRadius * 2.f, -loopRadius, 0.f);

    Segment s1{};
    s1.type = SegmentType::Line;
    s1.p0 = startPos;
    s1.p3 = loopEntry;
    m_segments.push_back(s1);

    // --- Segment 2: Single arc loop (same direction as EnteringState) ---
    glm::vec3 loopCenter = loopEntry + glm::vec3(-dir * loopRadius, 0.f, 0.f);
    float startAngle = fromLeft ? 0.f : glm::pi<float>();
    float sweepAngle = fromLeft ? glm::pi<float>() * 2.f : -glm::pi<float>() * 2.f; // full loop

    Segment s2{};
    s2.type = SegmentType::Arc;
    s2.center = loopCenter;
    s2.radius = loopRadius;
    s2.startAngle = startAngle;
    s2.sweepAngle = sweepAngle;
    m_segments.push_back(s2);

    // --- Segment 3: Straight descent from loop exit to mid-screen hover point ---
    float exitAngle = startAngle + sweepAngle;
    glm::vec3 loopExit = loopCenter + glm::vec3(
        std::cos(exitAngle) * loopRadius,
        std::sin(exitAngle) * loopRadius,
        0.f);

    Segment s3{};
    s3.type = SegmentType::Line;
    s3.p0 = loopExit;
    s3.p3 = midScreenPos;
    m_segments.push_back(s3);

    ComputeSegmentTimeRanges();
}

void dae::BossTractorApproachState::ComputeSegmentTimeRanges()
{
    float totalLength = 0.f;
    std::vector<float> lengths;

    for (const auto& seg : m_segments)
    {
        float len = seg.type == SegmentType::Line
            ? glm::length(seg.p3 - seg.p0)
            : std::abs(seg.sweepAngle) * seg.radius;
        lengths.push_back(len);
        totalLength += len;
    }

    float accumulated = 0.f;
    for (int i = 0; i < static_cast<int>(m_segments.size()); ++i)
    {
        m_segments[i].timeStart = accumulated / totalLength;
        accumulated += lengths[i];
        m_segments[i].timeEnd = accumulated / totalLength;
    }
}