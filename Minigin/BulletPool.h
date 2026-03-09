#pragma once
#include <vector>
#include <memory>
#include "Singleton.h"
#include "Components/Component.h"

namespace dae
{
    class GameObject;
    class Scene;
    class BulletPool final : public Singleton<BulletPool>
    {
    public:
        void Initialize(dae::Scene* scene, int poolSize);
        GameObject* GetBullet();
        void ReturnBullet(GameObject* bullet);

    private:
        friend class Singleton<BulletPool>;
        BulletPool() = default;
        std::vector<std::pair<GameObject*, bool>> m_bullets;
    };
}