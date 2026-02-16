#pragma once
#include "RenderComponent.h"
#include <string>
#include <memory>

namespace dae
{
	class Font;
	class Texture2D;

	class TextComponent final : public RenderComponent
	{
	public:
		void Update() override;

		void SetText(const std::string& text);
		void SetColor(const SDL_Color& color);

		TextComponent(const std::string& text, std::shared_ptr<Font> font, const SDL_Color& color);
		~TextComponent() override = default;
	private:
		bool m_needsUpdate{};
		std::string m_text{};
		SDL_Color m_color{ 255, 255, 255, 255 };
		std::shared_ptr<Font> m_font{};
	};
}