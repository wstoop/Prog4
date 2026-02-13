#include "FPSComponent.h"
#include "Renderer.h"
#include "../GameObject.h"
#include "TimeManager.h"
#include "TextComponent.h"
#include <sstream>
#include <iomanip>

void dae::FPSComponent::Update()
{
    const float dt = dae::TimeManager::GetInstance().GetDeltaTime();

    m_AccumulatedTime += dt;
    ++m_FrameCount;

    if (m_AccumulatedTime >= 0.2f)
    {
        float currentFPS = static_cast<float>(m_FrameCount) / m_AccumulatedTime;

        if (!m_TextComponent)
        {
            m_TextComponent = GetOwner()->GetComponent<TextComponent>();
        }

        if (m_TextComponent)
        {
            std::ostringstream ss;
            ss << std::fixed << std::setprecision(1) << currentFPS;
            m_TextComponent->SetText(ss.str() + " FPS");
        }
        m_AccumulatedTime = 0.0f;
        m_FrameCount = 0;
    }
}

