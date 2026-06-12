#pragma once
#include "Components/Component.h"
#include "EventManager.h"
#include <glm/glm.hpp>

namespace dae
{
    class ShootComponent;
    class TransformComponent;
    class RenderComponent;

    // The red "evil ship" escort that appears above a boss once it captures
    // the player via the tractor beam. On capture it spins while flying from
    // the player's position up to its spot above the boss; afterwards it
    // mirrors the boss's shots and is destroyed alongside it.
    class CapturedFighterAIComponent final : public Component, public EventHandler
    {
    public:
        CapturedFighterAIComponent(GameObject* owner, GameObject* boss, const glm::vec3& startPos);

        void Update() override;
        void HandleEvent(const Event* pEvent) override;

    private:
        glm::vec2 GetDockPosition() const;

        GameObject* m_boss;
        ShootComponent* m_shoot{ nullptr };
        TransformComponent* m_transform{ nullptr };

        glm::vec3 m_startPos;
        bool m_capturing{ true };
        float m_animTimer{ 0.f };

        static constexpr float k_animDuration{ 2.0f };
        // Total spin so the ship ends up facing the opposite way (180 degrees)
        // after a couple of full rotations.
        static constexpr float k_totalSpinDegrees{ 900.f };

        TransformComponent* m_bossTransform{};
        RenderComponent* m_bossRender{};
        RenderComponent* m_selfRender{};
    };
}
