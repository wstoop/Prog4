#include "EnemyEntryComponent.h"
#include "TransformComponent.h"
#include "FormationComponent.h"
#include "../GameObject.h"
#include "../TimeManager.h"
#include <cstdlib>

dae::EnemyEntryComponent::EnemyEntryComponent(GameObject* owner, GameObject* formationParent, TransformComponent* transform, const glm::vec3& target, float duration, float startDelay)
	: Component(owner)
	, m_duration(duration)
	, m_startDelay(startDelay)
	, m_endPoint(target)
	, m_transform(transform)
	, m_formationParent(formationParent)
{
}

void dae::EnemyEntryComponent::Update()
{
    if (m_done)
    {
        return;
    }
	if (!m_start)
    {
        return;
    }

    float dt = dae::TimeManager::GetInstance().GetDeltaTime();
    if (m_startDelay > 0.f)
    {
        m_startDelay -= dt;
        return;
    }
    m_elapsed += dt;
    float totalT = m_elapsed / m_duration;
    if (totalT >= 1.f)
    {
        m_transform->SetLocalPosition(m_endPoint);
        m_transform->SetParent(m_formationParent, true);
        m_formationParent->GetComponent<FormationComponent>()->EnemyDocked();
        m_transform->SetRotation(0.f, 0.f,0.f);
        m_done = true;
        return;
    }
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
    const auto& seg = *activeSeg;

    glm::vec3 pos{};
    float angle = 0.f;
    if (seg.type == SegmentType::Line)
    {
        pos = glm::mix(seg.p0, seg.p3, localT);
        glm::vec3 nextPos = glm::mix(seg.p0, seg.p3, localT + 0.001f);
        glm::vec3 dir = glm::normalize(nextPos - pos);
        angle = atan2(dir.y, dir.x);
    }
    else
    {
        float currentAngle = seg.startAngle + seg.sweepAngle * localT;
        pos = seg.center + glm::vec3(std::cos(currentAngle) * seg.radius, std::sin(currentAngle) * seg.radius, 0.f);
        glm::vec2 tangent(
            -std::sin(currentAngle),
            std::cos(currentAngle)
        );

        // Flip if sweep is negative
        if (seg.sweepAngle < 0)
            tangent = -tangent;

        angle = std::atan2(tangent.y, tangent.x);
    }
    m_transform->SetLocalPosition(pos);
    float deg = glm::degrees(angle);
    deg += 90.f;
    m_transform->SetRotation(0.f, 0.f, deg);
}

void dae::EnemyEntryComponent::BasicTopEntry(bool fromLeft)
{
    float radius = 60.f;
    float screenWidth = 800.f;
    float direction = fromLeft ? 1.f : -1.f;

    glm::vec3 start{};
    glm::vec3 turnEntry{};
    // Phase 0: diagonal entry
    if(fromLeft)
    {
        start = { screenWidth * 0.5f - 200.f, -100.f, 0.f };
        turnEntry = { screenWidth * 0.6f, 500.f, 0.f };
    }
    else
    {
        start = { screenWidth * 0.5f + 100.f, -100.f, 0.f };
        turnEntry = { screenWidth * 0.2f, 500.f, 0.f };
    }

    Segment lineSeg{};
    lineSeg.type = SegmentType::Line;
    lineSeg.p0 = start;
    lineSeg.p3 = turnEntry;
    m_segments.push_back(lineSeg);

    // Phase 1: loop
    glm::vec3 circleCenter = turnEntry + glm::vec3(-direction * radius, 0.f, 0.f);
    float startAngle = fromLeft ? 0.f : 3.14f;
    float sweepAngle = fromLeft ? 1.f * 3.14f : -1.f * 3.14f;
    
    Segment arcSeg{};
    arcSeg.type = SegmentType::Arc;
    arcSeg.center = circleCenter;
    arcSeg.radius = radius;
    arcSeg.startAngle = startAngle;
    arcSeg.sweepAngle = sweepAngle;
    m_segments.push_back(arcSeg);

    //Phase 3: exit loop and go to target position
    float exitAngle = startAngle + sweepAngle;
    glm::vec3 arcExit = circleCenter + glm::vec3(std::cos(exitAngle) * radius, std::sin(exitAngle) * radius, 0.f);
    Segment exitLine{};
    exitLine.type = SegmentType::Line;
    exitLine.p0 = arcExit;
    exitLine.p3 = m_endPoint;
    m_segments.push_back(exitLine);
    ComputeSegmentTimeRanges();
}

void dae::EnemyEntryComponent::BasicSideEntry(bool fromLeft)
{
    float radius = 60.f;
    float screenWidth = 800.f;
    float screenHeight = 600.f;

    float direction = fromLeft ? 1.f : -1.f;

    glm::vec3 start{};
    glm::vec3 turnEntry{};

    // Phase 0: horizontal entry from side
    if (fromLeft)
    {
        start = { -100.f, screenHeight * 0.6f, 0.f };
        turnEntry = { 300.f,  screenHeight * 0.7f, 0.f };
    }
    else
    {
        start = { screenWidth + 100.f, screenHeight * 0.6f, 0.f };
        turnEntry = { screenWidth - 300.f, screenHeight * 0.7f, 0.f };
    }

    Segment lineSeg{};
    lineSeg.type = SegmentType::Line;
    lineSeg.p0 = start;
    lineSeg.p3 = turnEntry;
    m_segments.push_back(lineSeg);

    // Phase 1: vertical half-loop (turning downward)
    glm::vec3 circleCenter = turnEntry + glm::vec3(0.f, direction * radius, 0.f);

    float startAngle = fromLeft ? -3.14f / 2.f : 3.14f / 2.f;
    float sweepAngle = fromLeft ? 3.14f : -3.14f;

    Segment arcSeg{};
    arcSeg.type = SegmentType::Arc;
    arcSeg.center = circleCenter;
    arcSeg.radius = radius;
    arcSeg.startAngle = startAngle;
    arcSeg.sweepAngle = sweepAngle;
    m_segments.push_back(arcSeg);

    // Phase 2: exit arc and go to target position
    float exitAngle = startAngle + sweepAngle;

    glm::vec3 arcExit = circleCenter +
        glm::vec3(std::cos(exitAngle) * radius,
            std::sin(exitAngle) * radius,
            0.f);

    Segment exitLine{};
    exitLine.type = SegmentType::Line;
    exitLine.p0 = arcExit;
    exitLine.p3 = m_endPoint;
    m_segments.push_back(exitLine);

    ComputeSegmentTimeRanges();
}

void dae::EnemyEntryComponent::ComputeSegmentTimeRanges()
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

void dae::EnemyEntryComponent::StartEntryTop(bool left)
{
    BasicTopEntry(left);
    m_start = true;
}

void dae::EnemyEntryComponent::StartEntrySide(bool left)
{
    BasicSideEntry(left);
    m_start = true;
}
