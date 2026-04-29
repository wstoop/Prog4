#pragma once
#include "Components/Component.h"
#include "EventManager.h"
#include <vector>

namespace dae
{
    class HitboxComponent final : public Component
    {
    public:
        HitboxComponent(GameObject* owner, float width, float height, float offsetX = 0.f, float offsetY = 0.f);
        ~HitboxComponent() override;

        void Update() override;

        float GetWidth() const { return m_width; }
        float GetHeight() const { return m_height; }
        float GetOffsetX() const { return m_offsetX; }
        float GetOffsetY() const { return m_offsetY; }

        bool Overlaps(const HitboxComponent* other) const;

    private:
        float m_width;
        float m_height;
        float m_offsetX;
        float m_offsetY;

        static std::vector<HitboxComponent*> s_allHitboxes;
    };
}
