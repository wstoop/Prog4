#pragma once
#include "Component.h"
#include <string>
#include <memory>

namespace dae
{
	class TextComponent;

	class FPSComponent final : public Component
	{
		std::shared_ptr<TextComponent> m_TextComponent;
		float m_AccumulatedTime{ 0.0f };
		int	m_FrameCount{ 0 };
	public:
		void Update() override;
		~FPSComponent() override = default;
	};
}