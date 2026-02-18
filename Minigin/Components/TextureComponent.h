#pragma once
#include "RenderComponent.h"
#include <memory>
#include <string>

namespace dae
{
	class Texture2D;

	class TextureComponent final : public RenderComponent
	{
	public:
		explicit TextureComponent(const std::shared_ptr<Texture2D>& texture);
		explicit TextureComponent(const std::string& textureFile);

		void SetTexture(const std::shared_ptr<Texture2D>& texture) { m_texture = texture; }
		std::shared_ptr<Texture2D> GetTexture() const { return m_texture; }
	};
}