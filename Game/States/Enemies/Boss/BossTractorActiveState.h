// States/BossStates/BossTractorActiveState.h
#pragma once
#include "../IEnemyState.h"

namespace dae {

    // Phase 2: Boss hovers at mid-screen and fires the tractor beam downward.
    //
    // Responsibilities:
    //   • Plays the tractor beam visual (via a TractorBeamComponent or similar)
    //   • Polls whether the player fighter overlaps the beam cone
    //   • If captured  → transitions to BossCapturingFighterState
    //   • If timeout   → reattaches to formation, transitions to InFormationState
    //
    // The beam stays active for m_beamDuration seconds.
    // During that window the beam component should be doing its own
    // overlap / trigger detection and calling NotifyCaptured() on this state.
    //
    class BossTractorActiveState final : public IEnemyState {
    public:
        void OnEnter(EnemyBrainComponent& brain) override;
        void Update(EnemyBrainComponent& brain) override;
        void OnExit(EnemyBrainComponent& brain) override;

        // Called by TractorBeamComponent (or whatever detects the overlap)
        // when the player fighter enters the beam.
        void NotifyCaptured() { m_captured = true; }

    private:
        void ReturnToFormation(EnemyBrainComponent& brain);

        float m_beamDuration{ 4.f };  // seconds beam stays open
        float m_elapsed{ 0.f };
        bool  m_captured{ false };
    };

}