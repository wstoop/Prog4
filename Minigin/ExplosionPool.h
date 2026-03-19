#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "GameObject.h"
#include "Singleton.h"

class GameObject;

class ExplosionPool final : public dae::Singleton<ExplosionPool>
{
public:
    void ActivateExplosion(const glm::vec3& position);
    void Update();
    void Render() const;
private:
    friend class Singleton<ExplosionPool>;
    ExplosionPool() = default;
    std::unique_ptr<dae::GameObject> CreateExplosion() const;
    std::vector<std::unique_ptr<dae::GameObject>> m_explosions;
};