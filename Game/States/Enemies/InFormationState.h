// States/InFormationState.h
#pragma once
#include "IEnemyState.h"

namespace dae {
    class InFormationState final : public IEnemyState {
        float m_diveTimer{ 0.f };
    public:
        void OnEnter(EnemyBrainComponent& brain) override;
        void Update(EnemyBrainComponent& brain) override;
    };
}