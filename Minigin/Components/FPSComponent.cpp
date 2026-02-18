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

    m_accumulatedTime += dt;
    ++m_frameCount;

    if (m_accumulatedTime >= 0.2f)
    {
        float currentFPS = static_cast<float>(m_frameCount) / m_accumulatedTime;

        if (!m_textComponent)
        {
            m_textComponent = GetOwner()->GetComponent<TextComponent>();
        }

        if (m_textComponent)
        {
            std::ostringstream ss;
            ss << std::fixed << std::setprecision(1) << currentFPS;
            m_textComponent->SetText(ss.str() + " FPS");
        }
        m_accumulatedTime = 0.0f;
        m_frameCount = 0;
    }
}

