#include "RotateAndScaleComponent.h"
#include "TransformComponent.h"
#include "TimeManager.h"
#include "../GameObject.h"

dae::RotateAndScaleComponent::RotateAndScaleComponent(GameObject* owner, float rotationSpeed, float scaleSpeed, float minScale, float maxScale)
	: Component(owner), m_rotationSpeed(rotationSpeed), m_scaleSpeed(scaleSpeed), m_minScale(minScale), m_maxScale(maxScale), m_scalingUp(true)
{
}

void dae::RotateAndScaleComponent::Update()
{
	const float dt = TimeManager::GetInstance().GetDeltaTime();

	if (auto transform = GetOwner()->GetComponent<TransformComponent>(); transform)
	{
		{
			auto rot = transform->GetRotation();
			rot.z += m_rotationSpeed * dt;
			if (rot.z >= 360.0f) rot.z = std::fmod(rot.z, 360.0f);
			else if (rot.z < 0.0f) rot.z = 360.0f + std::fmod(rot.z, 360.0f);
			transform->SetRotation(rot);
		}

		{
			auto scale = transform->GetScale();
			float s = scale.x;
			if (m_scalingUp)
			{
				s += m_scaleSpeed * dt;
				if (s >= m_maxScale)
				{
					s = m_maxScale;
					m_scalingUp = false;
				}
			}
			else
			{
				s -= m_scaleSpeed * dt;
				if (s <= m_minScale)
				{
					s = m_minScale;
					m_scalingUp = true;
				}
			}
			transform->SetScale(s, s, scale.z);
		}
	}
}