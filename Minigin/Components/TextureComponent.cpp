#include "TextureComponent.h"
#include "Texture2D.h"
#include "../Renderer.h"
#include "../GameObject.h"
#include "TransformComponent.h"
#include "../ResourceManager.h"

using namespace dae;

TextureComponent::TextureComponent(const std::shared_ptr<Texture2D>& texture)
{
	m_Texture = texture;
}

TextureComponent::TextureComponent(const std::string& textureFile)
{
	m_Texture = ResourceManager::GetInstance().LoadTexture(textureFile);
}
