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
            float frameTime = 0.1f,
            bool loop = true);

        void Update() override;

        void Play() { m_Playing = true;  m_CurrentFrame = 0; m_Accumulator = 0.0f;}
        void Stop() { m_Playing = false; }
        void SetRow(int row);
        void SetFrame(int frame);
        bool IsPlaying() const { return m_Playing; }
    private:
        void UpdateSourceRect();

        int m_Columns{};
        int m_Rows{};
        int m_CurrentFrame{};
        int m_CurrentRow{};

        float m_FrameTime{};
        float m_Accumulator{};
        bool m_Playing{ true };
        bool m_loop{};
    };
}