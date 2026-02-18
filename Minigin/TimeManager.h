#pragma once
#include "Singleton.h"

namespace dae
{
    class TimeManager final : public Singleton<TimeManager>
    {
        friend class Singleton<TimeManager>;
        TimeManager() = default;
        float m_deltaTime{};
    public:
        void SetDeltaTime(float deltaTime) { m_deltaTime = deltaTime; }
        float GetDeltaTime() const { return m_deltaTime; }
    };
}