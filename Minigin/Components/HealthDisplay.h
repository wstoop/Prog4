#pragma once
#include "Component.h"
#include "../GameObject.h"
#include "TransformComponent.h"
#include "../GameEvents.h"
#include "../EventManager.h"

namespace dae
{
    class HealthDisplay final : public Component, public EventHandler
    {
    public:
        HealthDisplay(GameObject* owner, GameObject* target)
            : Component(owner),
            m_target(target)
        {
            EventManager::GetInstance().AttachEvent(PLAYER_LOST_LIFE, this);
        }

        void HandleEvent(const Event* pEvent) override
        {
            if (pEvent->id != PLAYER_LOST_LIFE) return;

            const auto* e = static_cast<const DataEvent<LostLifeEvent>*>(pEvent);
            if (e->data.target != m_target) return;

            auto& children = GetOwner()->GetComponent<dae::TransformComponent>()->GetChildren();
            if (children.empty()) return;

            children.back()->m_destroy = true;
        }
    private:
        GameObject* m_target;
    };
}