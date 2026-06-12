#pragma once
#include "Components/Component.h"
#include "EventManager.h"

namespace dae
{
    class TextComponent;

    class LevelDisplayComponent final : public Component, public EventHandler
    {
    public:
        explicit LevelDisplayComponent(GameObject* owner);

        void HandleEvent(const Event* pEvent) override;

    private:
        TextComponent* m_text{ nullptr };
        int m_level{ 1 };
    };
}
