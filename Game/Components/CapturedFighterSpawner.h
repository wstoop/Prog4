#pragma once
#include "Components/Component.h"
#include "EventManager.h"

namespace dae
{
    class Scene;

    // Listens for EVENT_FIGHTER_CAPTURED and spawns the red "evil ship"
    // escort above the boss that captured the player.
    class CapturedFighterSpawner final : public Component, public EventHandler
    {
    public:
        CapturedFighterSpawner(GameObject* owner, Scene& scene);

        void HandleEvent(const Event* pEvent) override;

    private:
        Scene& m_scene;
    };
}
