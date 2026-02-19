#include "TextureComponent.h"
#include "Texture2D.h"
#include "../Renderer.h"
#include "../ResourceManager.h"

using namespace dae;

TextureComponent::TextureComponent(GameObject* owner, const std::shared_ptr<Texture2D>& texture):
	RenderComponent(owner)
{
	m_texture = texture;
}

TextureComponent::TextureComponent(GameObject* owner, const std::string& textureFile):
	RenderComponent(owner)
{
	m_texture = ResourceManager::GetInstance().LoadTexture(textureFile);
}
