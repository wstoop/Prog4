#pragma once
#include <vector>
#include <memory>
#include "Singleton.h"

namespace dae
{
    class GameObject;

    class BulletPool final : public Singleton<BulletPool>
    {
    public:
        GameObject* GetBullet();
        void ReturnBullet(GameObject* bullet);
        void Update();
        void Render() const;
    private:
        friend class Singleton<BulletPool>;
        BulletPool() = default;
        std::unique_ptr<GameObject> CreateBullet() const;
        std::vector<std::unique_ptr<GameObject>> m_bullets;
    };
}