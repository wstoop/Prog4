#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Singleton.h"

namespace dae
{
    class GameObject;

    class ExplosionPool final : public Singleton<ExplosionPool>
    {
    public:
        void ActivateExplosion(const glm::vec3& position);
        void ReturnExplosion(GameObject* bullet);
        void Update();
        void Render() const;
    private:
        friend class Singleton<ExplosionPool>;
        ExplosionPool() = default;
        std::unique_ptr<GameObject> CreateExplosion() const;
        std::vector<std::unique_ptr<GameObject>> m_explosions;
    };
}