#pragma once
#include "Component.h"
#include <glm/glm.hpp>

namespace dae
{
	class TransformComponent final : public Component
	{
	public:

		const glm::vec3& GetPos() const { return m_pos; }
		const glm::vec3& GetScale() const { return m_scale; }
		const glm::vec3& GetRotation() const { return m_rotation; }

		void SetPosition(const float x, const float y, const float z = 0);
		void SetPosition(const glm::vec3& position);

		void SetScale(const float x, const float y, const float z = 1);
		void SetScale(const glm::vec3& scale);

		void SetRotation(const float x, const float y, const float z = 0);
		void SetRotation(const glm::vec3& rotation);

		void Update() override {}
	private:
		glm::vec3 m_pos{};
		glm::vec3 m_scale{ 1.0f, 1.0f, 1.0f };
		glm::vec3 m_rotation{};
	};
}