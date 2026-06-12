#pragma once
#include "Components/Component.h"
#include "EventManager.h"

namespace dae
{
    class TextComponent;

    class HighScoreDisplayComponent final : public Component, public EventHandler
    {
    public:
        explicit HighScoreDisplayComponent(GameObject* owner);

        void HandleEvent(const Event* pEvent) override;

    private:
        TextComponent* m_text{ nullptr };
        int m_highScore{ 0 };
        int m_currentTotal{ 0 };
    };
}
