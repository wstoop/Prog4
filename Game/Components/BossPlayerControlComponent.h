#pragma once
#include "Components/Component.h"

namespace dae
{
    class BossPlayerControlComponent final : public Component
    {
    public:
        explicit BossPlayerControlComponent(GameObject* owner);

        void Update() override;

        void TriggerDiveBomb();
        void TriggerTractorBeam();

    private:
        bool CanAttack() const;

        float m_diveCooldown{ 0.f };
        float m_tractorCooldown{ 0.f };

        static constexpr float k_diveCooldownTime{ 5.f };
        static constexpr float k_tractorCooldownTime{ 8.f };
    };
}
