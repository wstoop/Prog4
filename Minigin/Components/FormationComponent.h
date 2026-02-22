#pragma once
#include "Component.h"
#include <glm/glm.hpp>

namespace dae
{
	class TransformComponent;
    class FormationComponent final : public Component
    {
        float m_time{ 0.f };
        float m_horizontalOffset{ 30.f };
        bool m_moveLeft{ true };
        TransformComponent* m_transform{ nullptr };
		int m_allEnemies{999};
		int m_dockedEnemies{0};
		void LeftRight();
    public:
        FormationComponent(GameObject* owner);
        
        void Update() override;
		void SetAllEnemies(int enemyCount) { m_allEnemies = enemyCount; }
		void EnemyDocked() { ++m_dockedEnemies; }
    };
}