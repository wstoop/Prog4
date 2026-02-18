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
		void Update() override;
		~FPSComponent() override = default;
	};
}