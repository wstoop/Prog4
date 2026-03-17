#pragma once
#include "Component.h"
#include "../GameEvents.h"

namespace dae
{
    class LivesComponent : public Component
    {
    public:
        LivesComponent(GameObject* owner, int lives)
            : Component(owner), m_Lives(lives) {
        }

        void LoseLife()
        { --m_Lives;
            EventManager::GetInstance().SendEvent(
            std::make_unique<DataEvent<LostLifeEvent>>(
                PLAYER_LOST_LIFE,
                LostLifeEvent{ GetOwner()}
            )
        );
        }
        int  GetLives()    const { return m_Lives; }
        bool IsGameOver()  const { return m_Lives <= 0; }

    private:
        int m_Lives;
    };
}
