#include "TractorBeamComponent.h"
#include "HitboxComponent.h"
#include "PlayerHealthComponent.h"
#include "GameEvents.h"
#include "GameObject.h"
#include "Components/TransformComponent.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Texture2D.h"
#include "TimeManager.h"
#include "GameInfo.h"
#include "States/Enemies/Boss/BossTractorActiveState.h"
#include <algorithm>

dae::TractorBeamComponent::TractorBeamComponent(GameObject* owner)
    : RenderComponent(owner)
{
    m_beamHitbox = owner->AddComponent<HitboxComponent>(0.f, 0.f, 0.f, 0.f);
    m_beamTexture = ResourceManager::GetInstance().LoadTexture("tractorBeam.png");
    EventManager::GetInstance().AttachEvent(DATEVENT_ACTOR_OVERLAPPED, this);
}

void dae::TractorBeamComponent::Update()
{
    if (!m_active) return;

    m_activeTime += TimeManager::GetInstance().GetDeltaTime();

    m_accumulator += TimeManager::GetInstance().GetDeltaTime();
    while (m_accumulator >= m_frameTime)
    {
        m_accumulator -= m_frameTime;
        m_currentFrame = (m_currentFrame + 1) % k_frameCount;
    }
}

void dae::TractorBeamComponent::Activate(BossTractorActiveState* state)
{
    m_active = true;
    m_pState = state;
    m_currentFrame = 0;
    m_accumulator = 0.f;
    m_activeTime = 0.f;

    // The boss's birdIdle.png frame is 15x16 px, rendered at 3x scale.
    // RenderComponent keeps the sprite centered while scaling, so the boss's
    // on-screen center sits at (pos + frameSize/2) - independent of scale -
    // and its bottom edge at (pos + frameSize*(scale+1)/2).
    const float bossFrameWidth = 15.f;
    const float bossFrameHeight = 16.f;
    const float bossScale = 3.f;

    const auto textureSize = m_beamTexture->GetSize();
    m_width = textureSize.x / static_cast<float>(k_frameCount);

    // Center the beam under the boss's on-screen horizontal center
    // (pos + bossFrameWidth/2) - otherwise the beam's hitbox (and visual)
    // sits offset from the boss, often missing the player entirely.
    const float offsetX = (bossFrameWidth - m_width) * 0.5f;
    const float offsetY = bossFrameHeight * (bossScale + 1.f) * 0.5f;

    // Stretch the beam all the way down to the bottom of the screen so it
    // can actually reach (and capture) the player.
    auto* transform = GetOwner()->GetComponent<TransformComponent>();
    const float screenHeight = static_cast<float>(GameInfo::GetInstance().GetScreenHeight());
    m_height = std::max(0.f, screenHeight - (transform->GetWorldPosition().y + offsetY));

    m_beamHitbox->SetSize(m_width, m_height);
    m_beamHitbox->SetOffset(offsetX, offsetY);
}

void dae::TractorBeamComponent::Deactivate()
{
    m_active = false;
    m_pState = nullptr;
    m_beamHitbox->SetSize(0.f, 0.f);
}

void dae::TractorBeamComponent::Render()
{
    if (!m_active || !m_beamTexture) return;

    auto* transform = GetOwner()->GetComponent<TransformComponent>();
    const auto& pos = transform->GetWorldPosition();

    const float dstY = pos.y + m_beamHitbox->GetOffsetY();
    const float dstHeight = m_beamHitbox->GetHeight();

    const auto textureSize = m_beamTexture->GetSize();
    const float frameWidth = textureSize.x / static_cast<float>(k_frameCount);
    const float frameHeight = textureSize.y;

    // Uniformly scale the beam frame so it fits the full beam length.
    const float scale = dstHeight / frameHeight;
    const float dstWidth = frameWidth * scale;

    const float hitboxCenterX = pos.x + m_beamHitbox->GetOffsetX() + m_beamHitbox->GetWidth() * 0.5f;
    const float dstX = hitboxCenterX - dstWidth * 0.5f;

    SDL_FRect srcRect{
        m_currentFrame * frameWidth,
        0.f,
        frameWidth,
        frameHeight
    };

    SDL_FRect dstRect{
        dstX,
        dstY,
        dstWidth,
        dstHeight
    };

    Renderer::GetInstance().RenderTexture(*m_beamTexture, srcRect, dstRect, 0.f);
}

void dae::TractorBeamComponent::HandleEvent(const Event* pEvent)
{
    if (pEvent->id != DATEVENT_ACTOR_OVERLAPPED) return;
    if (!m_active) return;
    if (m_activeTime < k_captureDelay) return;

    const auto* e = static_cast<const DataEvent<OverlapEvent>*>(pEvent);
    if (e->data.self != GetOwner()) return;
    if (e->data.other->tag != "Player") return;

    if (auto* health = e->data.other->GetComponent<PlayerHealthComponent>())
        health->Capture();

    if (m_pState)
        m_pState->NotifyCaptured(e->data.otherPos);
    Deactivate();
}
