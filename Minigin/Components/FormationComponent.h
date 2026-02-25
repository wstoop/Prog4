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

        std::vector<TransformComponent*> m_enemyTransforms;
        std::vector<glm::vec3> m_originalLocalPositions;
        glm::vec3 m_center{};

        float m_breathTime{};
        float m_breathSpeed{ 0.8f };
        float m_breathAmount{ 3.f };

		void LeftRight();
        void Breathe();
    public:
        FormationComponent(GameObject* owner);
        
        void Update() override;
        void SetAllEnemies(int enemyCount);
        void RegisterEnemy(TransformComponent* enemyTransform);
    };
}