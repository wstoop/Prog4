#pragma once
#include "Component.h"

namespace dae
{
	class RotateAndScaleComponent final : public Component
	{
	public:
		// rotationSpeed: degrees per second
		// scaleSpeed: units per second (applied to uniform X/Y scale)
		RotateAndScaleComponent(float rotationSpeed = 90.0f, float scaleSpeed = 0.5f, float minScale = 0.5f, float maxScale = 1.5f);
		virtual ~RotateAndScaleComponent() override = default;

		void Update() override;

	private:
		float m_RotationSpeed;
		float m_ScaleSpeed;
		float m_MinScale;
		float m_MaxScale;
		bool  m_ScalingUp;
	};
}