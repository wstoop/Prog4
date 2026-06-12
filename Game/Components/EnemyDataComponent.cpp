#include "EnemyDataComponent.h"
#include "Components/AnimationComponent.h"
#include "BulletComponent.h"
#include "EventManager.h"
#include "GameEvents.h"
#include "GameObject.h"

void dae::EnemyDataComponent::HandleEvent(const Event* pEvent)
{
    switch (pEvent->id)
    {
	case DATEVENT_ACTOR_OVERLAPPED:
    {
        const auto* e = static_cast<const DataEvent<OverlapEvent>*>(pEvent);
        if (e->data.self != GetOwner()) return;
        if (e->data.other->tag != "Bullet") return;
        if (e->data.other->GetComponent<BulletComponent>()->GetShooter()->tag != "Player") return;

        m_hit = true;
        GetOwner()->GetComponent<AnimationComponent>()->SetRow(1);
		break;
    }
	case EVENT_PLAYER_TOOK_DAMAGE:
    {
        const auto* e = static_cast<const DataEvent<PlayerTookDamageEvent>*>(pEvent);
        m_playerAlive = e->data.anyPlayerAlive;
		break;
    }
    case EVENT_ALL_ENEMIES_RETURNED:
		m_playerAlive = true;
		break;
    }
}