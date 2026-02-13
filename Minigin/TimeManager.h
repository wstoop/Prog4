#pragma once
#include "Singleton.h"

namespace dae
{
    class TimeManager final : public Singleton<TimeManager>
    {
    public:
        void SetDeltaTime(float deltaTime) { m_DeltaTime = deltaTime; }
        float GetDeltaTime() const { return m_DeltaTime; }

    private:
        friend class Singleton<TimeManager>;
        TimeManager() = default;
        float m_DeltaTime{};
    };
}