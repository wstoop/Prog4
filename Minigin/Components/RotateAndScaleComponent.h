#pragma once
#include "Component.h"

namespace dae
{
	class RotateAndScaleComponent final : public Component
	{
	public:
		RotateAndScaleComponent(float rotationSpeed = 90.0f, float scaleSpeed = 0.5f, float minScale = 0.5f, float maxScale = 1.5f);
		virtual ~RotateAndScaleComponent() override = default;

		void Update() override;

	private:
		float m_rotationSpeed;
		float m_scaleSpeed;
		float m_minScale;
		float m_maxScale;
		bool  m_scalingUp;
	};
}