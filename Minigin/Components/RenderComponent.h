#pragma once
#include <memory>
#include "Component.h"

namespace dae
{
	class Texture2D;

	class RenderComponent : public Component
	{
	public:
		RenderComponent(GameObject* owner);
		virtual ~RenderComponent() override = default;
		void Render() const;

		RenderComponent(const RenderComponent& other) = delete;
		RenderComponent(RenderComponent&& other) = delete;
		RenderComponent& operator=(const RenderComponent& other) = delete;
		RenderComponent& operator=(RenderComponent&& other) = delete;
	protected:
		std::shared_ptr<Texture2D> m_texture;
	};
}