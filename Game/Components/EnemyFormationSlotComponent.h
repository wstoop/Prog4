#pragma once
#include <glm/glm.hpp>
#include "Components/Component.h"

namespace dae
{
    class TransformComponent;
    class FormationComponent;

    class EnemyFormationSlotComponent final : public Component
    {
        FormationComponent* m_formation{ nullptr };
        glm::vec3 m_slotLocalPos{};
        glm::vec3 m_formationCenter{};
        bool m_notifiedDeath{ false };
        bool m_active{ false };          // ← track docked state

    public:
        EnemyFormationSlotComponent(GameObject* owner,
            FormationComponent* formation,
            const glm::vec3& slotLocalPos,
            const glm::vec3& formationCenter);
        ~EnemyFormationSlotComponent() override;

        void Activate();
        void Deactivate();               // ← new: call on dive start

        void NotifyIfDied();

        glm::vec3 ComputeSway() const;
        const glm::vec3& GetSlotLocalPos()  const { return m_slotLocalPos; }
        glm::vec3        GetSlotWorldPos()  const; // ← new: used by BeeSwoopState
        bool             IsActive()         const { return m_active; }
    };
}