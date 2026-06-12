#include "CapturedFighterSpawner.h"
#include "GameEvents.h"
#include "GameObject.h"
#include "Scene.h"

#include "Components/TransformComponent.h"
#include "Components/TextureComponent.h"
#include "Components/ShootComponent.h"
#include "Components/CapturedFighterAIComponent.h"
#include "Components/EnemyDataComponent.h"

#include <memory>

dae::CapturedFighterSpawner::CapturedFighterSpawner(GameObject* owner, Scene& scene)
    : Component(owner)
    , m_scene(scene)
{
    EventManager::GetInstance().AttachEvent(EVENT_FIGHTER_CAPTURED, this);
}

void dae::CapturedFighterSpawner::HandleEvent(const Event* pEvent)
{
    if (pEvent->id != EVENT_FIGHTER_CAPTURED) return;

    const auto* e = static_cast<const DataEvent<CaptureEvent>*>(pEvent);
    GameObject* boss = e->data.boss;

    auto* data = boss->GetComponent<EnemyDataComponent>();
    if (data == nullptr || data->GetEvilShip() != nullptr) return;

    auto go = std::make_unique<GameObject>();
    go->tag = "Enemy";

    go->AddComponent<TextureComponent>("evilPlayer.png");
    go->AddComponent<ShootComponent>(600.f);
    go->AddComponent<CapturedFighterAIComponent>(boss, e->data.startPos);
    go->GetComponent<TransformComponent>()->SetScale({ 3.f, 3.f, 0.f });
    go->GetComponent<TransformComponent>()->SetLocalPosition(e->data.startPos);

    GameObject* evilShip = go.get();
    data->SetEvilShip(evilShip);

    m_scene.Add(std::move(go));
}
