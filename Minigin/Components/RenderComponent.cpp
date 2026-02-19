#include "RenderComponent.h"
#include "TransformComponent.h"
#include "../GameObject.h"
#include "Renderer.h"
#include "Texture2D.h"

dae::RenderComponent::RenderComponent(GameObject* owner):
	Component(owner)
{
}

void dae::RenderComponent::Render() const
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

		const auto size = m_texture->GetSize();
		const float originalWidth = size.x;
		const float originalHeight = size.y;
		const float width = originalWidth * scaleX;
		const float height = originalHeight * scaleY;

		x -= (width - originalWidth) * 0.5f;
		y -= (height - originalHeight) * 0.5f;

		const float centerX = width * 0.5f;
		const float centerY = height * 0.5f;

		Renderer::GetInstance().RenderTexture(*m_texture, x, y, width, height, rot, centerX, centerY);
	}
}