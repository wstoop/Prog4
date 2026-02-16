#include "RotateAndScaleComponent.h"
#include "TransformComponent.h"
#include "TimeManager.h"
#include "../GameObject.h"
#include <algorithm>

dae::RotateAndScaleComponent::RotateAndScaleComponent(float rotationSpeed, float scaleSpeed, float minScale, float maxScale)
	: m_RotationSpeed(rotationSpeed), m_ScaleSpeed(scaleSpeed), m_MinScale(minScale), m_MaxScale(maxScale), m_ScalingUp(true)
{
}

void dae::RotateAndScaleComponent::Update()
{
	const float dt = TimeManager::GetInstance().GetDeltaTime();

	if (auto transform = GetOwner()->GetComponent<TransformComponent>(); transform)
	{
		{
			auto rot = transform->GetRotation();
			rot.z += m_RotationSpeed * dt;
			if (rot.z >= 360.0f) rot.z = std::fmod(rot.z, 360.0f);
			else if (rot.z < 0.0f) rot.z = 360.0f + std::fmod(rot.z, 360.0f);
			transform->SetRotation(rot);
		}

		{
			auto scale = transform->GetScale();
			float s = scale.x;
			if (m_ScalingUp)
			{
				s += m_ScaleSpeed * dt;
				if (s >= m_MaxScale)
				{
					s = m_MaxScale;
					m_ScalingUp = false;
				}
			}
			else
			{
				s -= m_ScaleSpeed * dt;
				if (s <= m_MinScale)
				{
					s = m_MinScale;
					m_ScalingUp = true;
				}
			}
			transform->SetScale(s, s, scale.z);
		}
	}
}