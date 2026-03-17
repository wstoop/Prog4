#pragma once
#include "Component.h"
#include "../EventManager.h"

namespace dae
{
    class TextComponent;

    class ScoreDisplayComponent final : public Component, public EventHandler
    {
        TextComponent* m_text{ nullptr };
        GameObject* m_trackedPlayer{ nullptr };
        int m_score{ 0 };

    public:
        ScoreDisplayComponent(GameObject* owner, GameObject* trackedPlayer);
        void HandleEvent(const Event* event) override;
    };
}
