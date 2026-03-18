#pragma once
#include <memory>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include "Component.h"

namespace dae
{
	class Texture2D;

	class RenderComponent : public Component
	{
	public:
		RenderComponent(GameObject* owner);
		virtual ~RenderComponent() override = default;
		virtual void Render();
		virtual glm::vec2 GetSize() const;
		RenderComponent(const RenderComponent& other) = delete;
		RenderComponent(RenderComponent&& other) = delete;
		RenderComponent& operator=(const RenderComponent& other) = delete;
		RenderComponent& operator=(RenderComponent&& other) = delete;
	protected:

		std::shared_ptr<Texture2D> m_texture;
		SDL_FRect m_SourceRect{0, 0, 0, 0};
		bool m_UseSourceRect{ false };
	};
}