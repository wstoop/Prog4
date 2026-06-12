#pragma once
#include "Components/RenderComponent.h"
#include "EventManager.h"
#include <memory>

namespace dae
{
    class HitboxComponent;
    class BossTractorActiveState;
    class Texture2D;

    class TractorBeamComponent final : public RenderComponent, public EventHandler
    {
    public:
        explicit TractorBeamComponent(GameObject* owner);

        void Update() override;
        void Render() override;
        void HandleEvent(const Event* pEvent) override;

        void Activate(BossTractorActiveState* state);
        void Deactivate();

    private:
        HitboxComponent* m_beamHitbox{ nullptr };
        BossTractorActiveState* m_pState{ nullptr };
        bool m_active{ false };

        // Grace period after activation during which the beam is visible but
        // can't capture the player yet, so they have a chance to dodge.
        float m_activeTime{ 0.f };
        static constexpr float k_captureDelay{ 0.5f };

        float m_width{ 45.f };
        float m_height{ 0.f };

        std::shared_ptr<Texture2D> m_beamTexture;
        static constexpr int k_frameCount{ 3 };
        int m_currentFrame{ 0 };
        float m_frameTime{ 0.1f };
        float m_accumulator{ 0.f };
    };
}
