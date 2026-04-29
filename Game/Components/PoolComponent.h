#pragma once
#include "Components/RenderComponent.h"
#include "BulletPool.h"
#include "ExplosionPool.h"

namespace dae
{
	class PoolComponent final : public dae::RenderComponent
    {
    public:
        explicit PoolComponent(dae::GameObject* owner)
            : RenderComponent(owner) {
        }

        void Update() override
        {
            BulletPool::GetInstance().Update();
            ExplosionPool::GetInstance().Update();
        }

        void Render() override
        {
            BulletPool::GetInstance().Render();
            ExplosionPool::GetInstance().Render();
        }
    };
}

