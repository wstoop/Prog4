// States/BossStates/BossAttackDecideState.cpp
#include "BossAttackDecideState.h"
#include "BossDiveBombState.h"
#include "BossTractorApproachState.h"
#include "../EnemyBrainComponent.h"
#include <random>

namespace {
    bool RollTractorBeam()
    {
        static std::mt19937 rng{ std::random_device{}() };
        // ~33 % chance of tractor beam, 67 % chance of dive bomb —
        // tweak the probability to taste.
        return std::bernoulli_distribution{ 0.33 }(rng);
    }
}

void dae::BossAttackDecideState::OnEnter(EnemyBrainComponent& brain)
{
    if (RollTractorBeam())
        brain.TransitionTo(std::make_unique<BossTractorApproachState>());
    else
        brain.TransitionTo(std::make_unique<BossDiveBombState>());
}