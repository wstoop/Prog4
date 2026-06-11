// States/EnteringState.cpp
#include "EnteringState.h"
#include "InFormationState.h"
#include "EnemyBrainComponent.h"
#include "Components/EnemyFormationSlotComponent.h"
#include "Components/TransformComponent.h"
#include "GameObject.h"
#include "TimeManager.h"
#include "GameInfo.h"
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// ---------- OnEnter: read config from brain, build path ----------

void dae::EnteringState::OnEnter(EnemyBrainComponent& brain)
{
    m_endPoint = brain.GetEntryTarget();
    m_duration = brain.GetEntryDuration();
    m_delay = brain.GetEntryDelay();
    m_elapsed = 0.f;
    m_done = false;
    m_segments.clear();

    BuildTopEntryPath(brain.GetEntryFromLeft(), m_endPoint);
}

// ---------- Update: interpolate along path ----------

void dae::EnteringState::Update(EnemyBrainComponent& brain)
{
    if (m_done) return;

    auto* owner = brain.GetOwner();
    auto* transform = owner->GetComponent<TransformComponent>();
    float dt = TimeManager::GetInstance().GetDeltaTime();

    // Handle death before docking
    if (owner->m_destroy)
    {
        if (auto* slot = owner->GetComponent<EnemyFormationSlotComponent>())
            slot->NotifyIfDied();
        m_done = true;
        return;
    }

    // Start delay
    if (m_delay > 0.f)
    {
        m_delay -= dt;
        return;
    }

    m_elapsed += dt;
    float totalT = m_elapsed / m_duration;

    // Arrived at destination
    if (totalT >= 1.f)
    {
        transform->SetLocalPosition(m_endPoint);
        transform->SetParent(brain.GetFormationParent(), true);
        transform->SetRotation(0.f, 0.f, 0.f);

        if (auto* slot = owner->GetComponent<EnemyFormationSlotComponent>())
            slot->Activate();

        m_done = true;
        brain.TransitionTo(std::make_unique<InFormationState>());
        return;
    }

    // Find active segment
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

    // Evaluate position + heading
    glm::vec3 pos{};
    float angle = 0.f;

    if (activeSeg->type == SegmentType::Line)
    {
        pos = glm::mix(activeSeg->p0, activeSeg->p3, localT);
        glm::vec3 nextPos = glm::mix(activeSeg->p0, activeSeg->p3, localT + 0.001f);
        glm::vec3 dir = glm::normalize(nextPos - pos);
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
}

// ---------- Path building ----------

void dae::EnteringState::BuildTopEntryPath(bool fromLeft, const glm::vec3& endPoint)
{
    const float radius = 60.f;
    const float horizontalOffset = 200.f;
    const float direction = fromLeft ? 1.f : -1.f;
    const float halfWidth = static_cast<float>(GameInfo::GetInstance().GetGameWidth()) / 2.f;

    glm::vec3 start = { halfWidth - horizontalOffset * direction, -100.f, 0.f };
    glm::vec3 turnEntry = { halfWidth + horizontalOffset * direction,  500.f, 0.f };

    // Line: spawn point → loop entry
    Segment lineSeg{};
    lineSeg.type = SegmentType::Line;
    lineSeg.p0 = start;
    lineSeg.p3 = turnEntry;
    m_segments.push_back(lineSeg);

    // Arc: loop
    glm::vec3 circleCenter = turnEntry + glm::vec3(-direction * radius, 0.f, 0.f);
    float startAngle = fromLeft ? 0.f : glm::pi<float>();
    float sweepAngle = fromLeft ? glm::pi<float>() : -glm::pi<float>();

    Segment arcSeg{};
    arcSeg.type = SegmentType::Arc;
    arcSeg.center = circleCenter;
    arcSeg.radius = radius;
    arcSeg.startAngle = startAngle;
    arcSeg.sweepAngle = sweepAngle;
    m_segments.push_back(arcSeg);

    // Line: arc exit → formation slot
    float     exitAngle = startAngle + sweepAngle;
    glm::vec3 arcExit = circleCenter + glm::vec3(
        std::cos(exitAngle) * radius,
        std::sin(exitAngle) * radius, 0.f);

    Segment exitLine{};
    exitLine.type = SegmentType::Line;
    exitLine.p0 = arcExit;
    exitLine.p3 = endPoint;
    m_segments.push_back(exitLine);

    ComputeSegmentTimeRanges();
}

void dae::EnteringState::ComputeSegmentTimeRanges()
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