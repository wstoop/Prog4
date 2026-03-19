#include "ScoreDisplayComponent.h"
#include "TextComponent.h"
#include "../GameObject.h"
#include "../Steam/Achievement.h"

struct ScoreEvent
{
    dae::GameObject* player;
    int points;
};

dae::ScoreDisplayComponent::ScoreDisplayComponent(GameObject* owner, GameObject* trackedPlayer)
    : Component(owner)
    , m_trackedPlayer(trackedPlayer)
{
    m_text = owner->GetComponent<TextComponent>();
    EventManager::GetInstance().AttachEvent(make_sdbm_hash("ScoreChanged"), this);
}

void dae::ScoreDisplayComponent::HandleEvent(const Event* event)
{
    const auto* scoreEvent = dynamic_cast<const DataEvent<ScoreEvent>*>(event);
    if (!scoreEvent) return;
    if (scoreEvent->data.player != m_trackedPlayer) return;

    m_score += scoreEvent->data.points;
    m_text->SetText(std::to_string(m_score));
#if USE_STEAMWORKS
    if (m_score >= 500)
        g_SteamAchievements->SetAchievement("ACH_WIN_ONE_GAME");
#endif
}