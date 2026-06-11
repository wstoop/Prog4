// States/BeeStates/BeeDiveBombState.h
#pragma once
#include "../IEnemyState.h"
#include <glm/glm.hpp>
#include <vector>

namespace dae {
    class BeeDiveBombState final : public IEnemyState {
        glm::vec3 m_p0{}, m_p1{}, m_p2{}, m_p3{};
        float     m_duration{ 2.2f };
        float     m_elapsed{ 0.f };

        bool      m_doSwoop{ false };
        bool      m_detached{ false };

        std::vector<float> m_shootTimers;

        glm::vec3 EvalBezier(float t) const;
        glm::vec3 EvalBezierTangent(float t) const; // <--- Add this

    public:
        void OnEnter(EnemyBrainComponent& brain) override;
        void Update(EnemyBrainComponent& brain) override;
        void OnExit(EnemyBrainComponent& brain) override;
    };
}