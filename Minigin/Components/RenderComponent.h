#pragma once
#include <memory>
#include <SDL3/SDL.h>
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

		RenderComponent(const RenderComponent& other) = delete;
		RenderComponent(RenderComponent&& other) = delete;
		RenderComponent& operator=(const RenderComponent& other) = delete;
		RenderComponent& operator=(RenderComponent&& other) = delete;
	protected:
		struct Rect
		{
			float x{};
			float y{};
			float width{};
			float height{};
		};

		std::shared_ptr<Texture2D> m_texture;
		SDL_FRect m_SourceRect{0, 0, 0, 0};
		bool m_UseSourceRect{ false };
	};
}