#include "LevelDisplayComponent.h"
#include "Components/TextComponent.h"
#include "GameEvents.h"
#include "GameObject.h"

dae::LevelDisplayComponent::LevelDisplayComponent(GameObject* owner)
    : Component(owner)
{
    m_text = owner->GetComponent<TextComponent>();
    m_text->SetText(std::to_string(m_level));

    EventManager::GetInstance().AttachEvent(EVENT_WAVE_CLEARED, this);
}

void dae::LevelDisplayComponent::HandleEvent(const Event* pEvent)
{
    if (pEvent->id != EVENT_WAVE_CLEARED) return;

    ++m_level;
    m_text->SetText(std::to_string(m_level));
}
