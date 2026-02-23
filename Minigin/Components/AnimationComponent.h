#pragma once
#include "RenderComponent.h"
#include <string>

namespace dae
{
    class AnimationComponent final : public RenderComponent
    {
    public:
        AnimationComponent(GameObject* owner,
            const std::string& texture,
            int columns,
            int rows = 1,
            float frameTime = 0.1f);

        void Update() override;

        void Play() { m_Playing = true; }
        void Stop() { m_Playing = false; }
        void SetRow(int row);
        void SetFrame(int frame);

    private:
        void UpdateSourceRect();

        int m_Columns{};
        int m_Rows{};
        int m_CurrentFrame{};
        int m_CurrentRow{};

        float m_FrameTime{};
        float m_Accumulator{};
        bool m_Playing{ true };
    };
}