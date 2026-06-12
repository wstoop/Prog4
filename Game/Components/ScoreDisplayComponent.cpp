#include "ScoreDisplayComponent.h"
#include "Components/TextComponent.h"
#include "GameObject.h"
#include "GameEvents.h"

dae::ScoreDisplayComponent::ScoreDisplayComponent(GameObject* owner, GameObject* trackedPlayer)
    : Component(owner)
    , m_trackedPlayer(trackedPlayer)
{
    m_text = owner->GetComponent<TextComponent>();
    EventManager::GetInstance().AttachEvent(DATEVENT_SCORE_CHANGED, this);
}

void dae::ScoreDisplayComponent::HandleEvent(const Event* event)
{
    const auto* scoreEvent = dynamic_cast<const DataEvent<ScoreEvent>*>(event);
    if (!scoreEvent) return;
    if (scoreEvent->data.killer != m_trackedPlayer) return;

    m_score += scoreEvent->data.points;
    m_text->SetText(std::to_string(m_score));
}