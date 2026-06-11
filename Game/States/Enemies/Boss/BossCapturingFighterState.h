// States/BossStates/BossCapturingFighterState.h
#pragma once
#include "../IEnemyState.h"
#include <glm/glm.hpp>

namespace dae {

    // Phase 3: Player has been caught in the beam.
    //
    // Sequence:
    //   1. Fighter turns red (handled by PlayerComponent / event)
    //   2. Fighter smoothly floats upward along the beam toward the Boss
    //   3. Once the fighter reaches the Boss, it docks next to it in formation
    //      as a new enemy GameObject that shoots at the player.
    //   4. Boss returns to formation  →  InFormationState
    //
    // The captured fighter becoming a new enemy is done by spawning/converting
    // through an event or a CaptureManager — the state fires that event and
    // then cleans up its own side.
    //
    class BossCapturingFighterState final : public IEnemyState {
    public:
        void OnEnter(EnemyBrainComponent& brain) override;
        void Update(EnemyBrainComponent& brain) override;
        void OnExit(EnemyBrainComponent& brain) override;

    private:
        // How long the "pull up" animation lasts before docking
        float m_pullDuration{ 2.0f };
        float m_elapsed{ 0.f };
        bool  m_docked{ false };
    };

}