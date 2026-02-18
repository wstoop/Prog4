#include "TextureComponent.h"
#include "Texture2D.h"
#include "../Renderer.h"
#include "../ResourceManager.h"

using namespace dae;

TextureComponent::TextureComponent(const std::shared_ptr<Texture2D>& texture)
{
	m_texture = texture;
}

TextureComponent::TextureComponent(const std::string& textureFile)
{
	m_texture = ResourceManager::GetInstance().LoadTexture(textureFile);
}
