#include "AnimationComponent.h"
#include "../GameObject.h"
#include "Texture2D.h"
#include "../TimeManager.h"
#include "../ResourceManager.h"

dae::AnimationComponent::AnimationComponent(GameObject* owner,
    const std::string& texture,
    int columns,
    int rows,
    float frameTime)
    : RenderComponent(owner)
    , m_Columns(columns)
    , m_Rows(rows)
    , m_FrameTime(frameTime)
{
       m_texture = ResourceManager::GetInstance().LoadTexture(texture);
       m_UseSourceRect = true;
       UpdateSourceRect();
}

void dae::AnimationComponent::Update()
{
    if (!m_Playing || m_Columns <= 1)
        return;
    m_Accumulator += TimeManager::GetInstance().GetDeltaTime();
    while (m_Accumulator >= m_FrameTime)
    {
        m_Accumulator -= m_FrameTime;
        m_CurrentFrame = (m_CurrentFrame + 1) % m_Columns;
    }
    UpdateSourceRect();
}

void dae::AnimationComponent::SetRow(int row)
{
    if (row < 0 || row >= m_Rows)
        return;
    m_CurrentRow = row;
    UpdateSourceRect();
}

void dae::AnimationComponent::SetFrame(int frame)
{
    if (frame < 0 || frame >= m_Columns)
        return;
    m_CurrentFrame = frame;
    UpdateSourceRect();
}

void dae::AnimationComponent::UpdateSourceRect()
{
    if (!m_texture)
        return;
    const auto size = m_texture->GetSize();
    const float frameWidth = size.x / static_cast<float>(m_Columns);
    const float frameHeight = size.y / static_cast<float>(m_Rows);
    m_SourceRect.x = m_CurrentFrame * frameWidth;
    m_SourceRect.y = m_CurrentRow * frameHeight;
    m_SourceRect.w = frameWidth;
    m_SourceRect.h = frameHeight;
}