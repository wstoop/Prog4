// States/EnteringState.h
#pragma once
#include "IEnemyState.h"
#include <vector>
#include <glm/glm.hpp>

namespace dae {

    class EnteringState final : public IEnemyState {
    public:
        void OnEnter(EnemyBrainComponent& brain) override;
        void Update(EnemyBrainComponent& brain) override;

    private:
        enum class SegmentType { Line, Arc };

        struct Segment {
            SegmentType type{};
            // Line
            glm::vec3 p0{}, p3{};
            // Arc
            glm::vec3 center{};
            float radius{};
            float startAngle{};
            float sweepAngle{};
            // Normalised time range [0,1] along total path
            float timeStart{};
            float timeEnd{};
        };

        void BuildTopEntryPath(bool fromLeft, const glm::vec3& endPoint);
        void ComputeSegmentTimeRanges();

        std::vector<Segment> m_segments;
        glm::vec3  m_endPoint{};
        float      m_duration{ 2.f };
        float      m_delay{ 0.f };
        float      m_elapsed{ 0.f };
        bool       m_done{ false };
    };

}