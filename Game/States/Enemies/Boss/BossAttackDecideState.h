// States/BossStates/BossAttackDecideState.h
#pragma once
#include "../IEnemyState.h"

namespace dae {

    // Instant decision node — no visual behaviour of its own.
    // Rolls a coin and immediately transitions to either
    // BossDiveBombState or BossTractorApproachState.
    class BossAttackDecideState final : public IEnemyState {
    public:
        void OnEnter(EnemyBrainComponent& brain) override;
        void Update(EnemyBrainComponent& /*brain*/) override {}   // never ticks
    };

}