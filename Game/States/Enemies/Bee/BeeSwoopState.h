// States/BeeStates/BeeSwoopState.h
#pragma once
#include "../IEnemyState.h"
#include <glm/glm.hpp>
#include <vector>

namespace dae {
    class BeeSwoopState final : public IEnemyState {
    public:
        void OnEnter(EnemyBrainComponent& brain) override;
        void Update(EnemyBrainComponent& brain) override;
        void OnExit(EnemyBrainComponent& brain) override;

    private:
        // Cubic Bézier: p0 = start (world), p1/p2 = control, p3 = end (formation slot)
        glm::vec3 m_p0{}, m_p1{}, m_p2{}, m_p3{};
        float m_duration{ 3.4f };
        float m_elapsed{ 0.f };

        std::vector<float> m_shootTimers;

        glm::vec3 EvalBezier(float t) const;
        glm::vec3 EvalBezierTangent(float t) const;
    };
}