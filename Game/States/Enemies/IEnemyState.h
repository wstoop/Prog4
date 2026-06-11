#pragma once
namespace dae {
    class EnemyBrainComponent;

    class IEnemyState {
    public:
        virtual ~IEnemyState() = default;
        virtual void OnEnter(EnemyBrainComponent&) {}
        virtual void Update(EnemyBrainComponent& brain) = 0;
        virtual void OnExit(EnemyBrainComponent&) {}
    };
}