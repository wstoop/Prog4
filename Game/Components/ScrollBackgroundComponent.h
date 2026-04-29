#pragma once
#include "Components/Component.h"

namespace dae
{
    class TransformComponent;

    class ScrollBackgroundComponent : public Component
    {
    public:
        ScrollBackgroundComponent(GameObject* owner, float scrollSpeed, float imageHeight);
        void Update() override;

    private:
        TransformComponent* m_transformA{ nullptr };
        TransformComponent* m_transformB{ nullptr };
        float m_scrollSpeed{};
        float m_imageHeight{};
    };
}