#include "EnemyDataComponent.h"
#include "AnimationComponent.h"
#include "BulletComponent.h"
#include "../EventManager.h"
#include "../GameEvents.h"
#include "../GameObject.h"

void dae::EnemyDataComponent::HandleEvent(const Event* pEvent)
{
    if (m_hit) return;

    const auto* e = static_cast<const DataEvent<OverlapEvent>*>(pEvent);
    if (e->data.self != GetOwner()) return;
    if (e->data.other->tag != "Bullet") return;

    m_hit = true;
    GetOwner()->GetComponent<AnimationComponent>()->SetRow(1);
}