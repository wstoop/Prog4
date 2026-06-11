// States/BossStates/BossDiveBombState.h
#pragma once
#include "../IEnemyState.h"
#include <glm/glm.hpp>
#include <vector>

namespace dae {

    // Mirrors BeeDiveBombState closely — cubic Bézier dive, no swoop variant.
    // Shoots 3 times on the way down, then reattaches to formation.
    class BossDiveBombState final : public IEnemyState {
    public:
        void OnEnter(EnemyBrainComponent& brain) override;
        void Update(EnemyBrainComponent& brain) override;
        void OnExit(EnemyBrainComponent& brain) override;

    private:
        glm::vec3 EvalBezier(float t) const;
        glm::vec3 EvalBezierTangent(float t) const;

        glm::vec3 m_p0{}, m_p1{}, m_p2{}, m_p3{};
        float     m_duration{ 3.6f };   // Bosses are a bit slower / more menacing
        float     m_elapsed{ 0.f };
        bool      m_detached{ false };

        std::vector<float> m_shootTimers;
    };

}