#include "TextureComponent.h"
#include "Texture2D.h"
#include "../Renderer.h"
#include "../GameObject.h"
#include "TransformComponent.h"
#include "../ResourceManager.h"

using namespace dae;

TextureComponent::TextureComponent(const std::shared_ptr<Texture2D>& texture)
	: m_texture(texture)
{
}

TextureComponent::TextureComponent(const std::string& textureFile)
{
	m_texture = ResourceManager::GetInstance().LoadTexture(textureFile);
}

void TextureComponent::Render() const
{
	if (!m_texture || GetOwner() == nullptr)
		return;

	auto transformComp = GetOwner()->GetComponent<TransformComponent>();
	float x = 0.0f;
	float y = 0.0f;
	if (transformComp)
	{
		const auto& pos = transformComp->GetTransform().GetPosition();
		x = pos.x;
		y = pos.y;
	}

	Renderer::GetInstance().RenderTexture(*m_texture, x, y);
}