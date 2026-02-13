#pragma once
#include "../Transform.h"
#include "Component.h"

namespace dae
{
	class TransformComponent final : public Component
	{
	public:
		TransformComponent() = default;

		const Transform& GetTransform() const { return m_transform; }
		Transform& GetTransform() { return m_transform; }
		
		void SetPosition(float x, float y, float z = 0) { m_transform.SetPosition(x, y, z); }
		const glm::vec3& GetPosition() const { return m_transform.GetPosition(); }

		void Update() override {}
	private:
		Transform m_transform{};
	};
}