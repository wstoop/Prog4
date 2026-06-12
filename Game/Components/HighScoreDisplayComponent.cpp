#include "HighScoreDisplayComponent.h"
#include "Components/TextComponent.h"
#include "HighScoreManager.h"
#include "GameEvents.h"
#include "GameObject.h"

dae::HighScoreDisplayComponent::HighScoreDisplayComponent(GameObject* owner)
    : Component(owner)
{
    m_text = owner->GetComponent<TextComponent>();

    const auto entries = HighScoreManager::Load();
    if (!entries.empty())
        m_highScore = entries.front().score;

    m_text->SetText(std::to_string(m_highScore));

    EventManager::GetInstance().AttachEvent(DATEVENT_SCORE_CHANGED, this);
}

void dae::HighScoreDisplayComponent::HandleEvent(const Event* pEvent)
{
    const auto* scoreEvent = dynamic_cast<const DataEvent<ScoreEvent>*>(pEvent);
    if (!scoreEvent) return;

    m_currentTotal += scoreEvent->data.points;

    if (m_currentTotal > m_highScore)
    {
        m_highScore = m_currentTotal;
        m_text->SetText(std::to_string(m_highScore));
    }
}
