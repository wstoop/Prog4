// States/BossStates/BossTractorApproachState.h
#pragma once
#include "../IEnemyState.h"
#include <glm/glm.hpp>
#include <vector>

namespace dae {

    // Phase 1 of the tractor beam run:
    //   • Does one small loop at the top of the screen (like the entry loop)
    //   • Then slides straight down to mid-screen
    //   • Transitions to BossTractorActiveState
    class BossTractorApproachState final : public IEnemyState {
    public:
        void OnEnter(EnemyBrainComponent& brain) override;
        void Update(EnemyBrainComponent& brain) override;
        void OnExit(EnemyBrainComponent& brain) override;

    private:
        enum class SegmentType { Line, Arc };

        struct Segment {
            SegmentType type{};
            glm::vec3 p0{}, p3{};       // line
            glm::vec3 center{};          // arc
            float radius{};
            float startAngle{};
            float sweepAngle{};
            float timeStart{};
            float timeEnd{};
        };

        void BuildApproachPath(const glm::vec3& startPos, const glm::vec3& midScreenPos);
        void ComputeSegmentTimeRanges();

        std::vector<Segment> m_segments;
        glm::vec3 m_destination{};      // mid-screen hover position
        float m_duration{ 2.4f };
        float m_elapsed{ 0.f };
        bool  m_detached{ false };
    };

}