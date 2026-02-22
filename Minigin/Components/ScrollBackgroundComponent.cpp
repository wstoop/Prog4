#include "ScrollBackgroundComponent.h"
#include "TransformComponent.h"
#include "../GameObject.h"
#include "../TimeManager.h"

dae::ScrollBackgroundComponent::ScrollBackgroundComponent(GameObject* owner, float scrollSpeed, float imageHeight)
    : Component(owner)
    , m_scrollSpeed(scrollSpeed)
    , m_imageHeight(imageHeight)
{
    auto children = owner->GetComponent<dae::TransformComponent>()->GetChildren();
    if (children.size() >= 2)
    {
        m_transformA = children[0]->GetComponent<TransformComponent>();
        m_transformB = children[1]->GetComponent<TransformComponent>();

        m_transformA->SetLocalPosition({ 0.f, 0.f, 0.f });
        m_transformB->SetLocalPosition({ 0.f, -m_imageHeight, 0.f });
    }
}

void dae::ScrollBackgroundComponent::Update()
{
    if (!m_transformA || !m_transformB)
        return;

    float dt = TimeManager::GetInstance().GetDeltaTime();
    float delta = m_scrollSpeed * dt;

    auto posA = m_transformA->GetLocalPosition();
    auto posB = m_transformB->GetLocalPosition();

    posA.y += delta;
    posB.y += delta;

    if (posA.y >= m_imageHeight)
        posA.y = posB.y - m_imageHeight;
    if (posB.y >= m_imageHeight)
        posB.y = posA.y - m_imageHeight;

    m_transformA->SetLocalPosition(posA);
    m_transformB->SetLocalPosition(posB);
}