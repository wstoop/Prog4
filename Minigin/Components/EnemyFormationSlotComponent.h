#pragma once
#include <glm/glm.hpp>
#include "Component.h"

namespace dae
{
    class TransformComponent;
    class FormationComponent;

    class EnemyFormationSlotComponent final : public Component
    {
        TransformComponent* m_transform{ nullptr };
        FormationComponent* m_formation{ nullptr };

        bool m_active{ false };
        bool m_notifiedDeath{ false };

    public:
        EnemyFormationSlotComponent(GameObject* owner,
            FormationComponent* formation);
		~EnemyFormationSlotComponent() override;
        void Update() override;

        // Called by EnemyEntryComponent once the entry animation finishes
        void Activate();

        // Called by EnemyEntryComponent when the owner dies before docking
        void NotifyIfDied();
    };
}