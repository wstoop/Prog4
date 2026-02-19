#pragma once
#include "Component.h"
#include <glm/glm.hpp>

namespace dae
{
	class TransformComponent final : public Component
	{
		void AddChild(GameObject* child);
		void RemoveChild(GameObject* child);
		bool IsChild(GameObject* child) const;
		void SetPositionDirty();
		glm::vec3 m_worldPosition{};
		glm::vec3 m_localPosition{};

		GameObject* m_parent = nullptr;
		bool m_positionIsDirty = true;
		std::vector<GameObject*> m_children{};

		glm::vec3 m_pos{};
		glm::vec3 m_scale{ 1.0f, 1.0f, 1.0f };
		glm::vec3 m_rotation{};
	public:
		TransformComponent(GameObject* owner);
		
		const glm::vec3& GetPos() const { return m_pos; }
		const glm::vec3& GetScale() const { return m_scale; }
		const glm::vec3& GetRotation() const { return m_rotation; }

		void SetScale(const float x, const float y, const float z = 1);
		void SetScale(const glm::vec3& scale);

		void SetRotation(const float x, const float y, const float z = 0);
		void SetRotation(const glm::vec3& rotation);


		void SetParent(GameObject* parent, bool keepWorldPosition);
		void SetLocalPosition(const glm::vec3& pos);
		const glm::vec3& GetWorldPosition();
		void UpdateWorldPosition();
		GameObject* GetParent();


		void Update() override {}
	};
}