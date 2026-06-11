#pragma once
#include "../IEnemyState.h"
#include <glm/glm.hpp>
#include <vector>

namespace dae {
    class ButterflyDiveBombState final : public IEnemyState {
        std::vector<glm::vec3> m_waypoints;
        size_t                 m_currentWaypointIdx{ 0 };
        float                  m_speed{ 400.f };

        bool                   m_doSwoop{ false };
        bool                   m_detached{ false };
        std::vector<float>     m_shootTimers;

    public:
        void OnEnter(EnemyBrainComponent& brain) override;
        void Update(EnemyBrainComponent& brain) override;
        void OnExit(EnemyBrainComponent& brain) override;
    };
}