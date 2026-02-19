#pragma once
#include "Component.h"
#include <memory>

namespace dae
{
	class TextComponent;

	class FPSComponent final : public Component
	{
		std::shared_ptr<TextComponent> m_textComponent;
		float m_accumulatedTime{ 0.0f };
		int	m_frameCount{ 0 };
	public:
		FPSComponent(GameObject* owner);
		~FPSComponent() override = default;
		void Update() override;
		FPSComponent(const FPSComponent& other) = delete;
		FPSComponent(FPSComponent&& other) = delete;
		FPSComponent& operator=(const FPSComponent& other) = delete;
		FPSComponent& operator=(FPSComponent&& other) = delete;
	};
}