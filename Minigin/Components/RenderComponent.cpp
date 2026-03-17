#include "RenderComponent.h"
#include "TransformComponent.h"
#include "../GameObject.h"
#include "Renderer.h"
#include "Texture2D.h"

dae::RenderComponent::RenderComponent(GameObject* owner):
	Component(owner)
{
}

void dae::RenderComponent::Render()
{
	if (m_texture != nullptr)
	{
		auto transformComp = GetOwner()->GetComponent<TransformComponent>();
		float x = 0.0f;
		float y = 0.0f;
		float rot = 0.0f;
		float scaleX = 1.0f;
		float scaleY = 1.0f;
		if (transformComp)
		{
			const auto& pos = transformComp->GetWorldPosition();
			const auto& scale = transformComp->GetScale();
			const auto& rotation = transformComp->GetRotation();

			x = pos.x;
			y = pos.y;
			scaleX = scale.x;
			scaleY = scale.y;
			rot = rotation.z;
		}

		float frameWidth{};
		float frameHeight{};

		if (m_UseSourceRect)
		{
			frameWidth = m_SourceRect.w;
			frameHeight = m_SourceRect.h;
		}
		else
		{
			const auto size = m_texture->GetSize();
			frameWidth = size.x;
			frameHeight = size.y;
		}

		const float width = frameWidth * scaleX;
		const float height = frameHeight * scaleY;

		x -= (width - frameWidth) * 0.5f;
		y -= (height - frameHeight) * 0.5f;

		const float centerX = width * 0.5f;
		const float centerY = height * 0.5f;

		SDL_FRect destRect{x, y, width, height};
		if (m_UseSourceRect)
		{
			Renderer::GetInstance(). RenderTexture(
				*m_texture,
				m_SourceRect,
				destRect,
				rot,
				centerX,
				centerY
				);
		}
		else
		{
			Renderer::GetInstance().RenderTexture(
				*m_texture,
				destRect,
				rot,
				centerX,
				centerY);
		}
	}
}

glm::vec2 dae::RenderComponent::GetSize() const
{
	return m_texture->GetSize();
}