#include <algorithm>
#include "GameObject.h"
#include "Components/Component.h"
#include "Components/RenderComponent.h"

dae::GameObject::~GameObject() = default;

void dae::GameObject::Update()
{
    std::for_each(m_components.begin(), m_components.end(), [](const auto& component) {
        component->Update();
    });
}

void dae::GameObject::LateUpdate()
{
    std::for_each(m_components.begin(), m_components.end(), [](const auto& component) {
        component->LateUpdate();
    });
}

void dae::GameObject::FixedUpdate()
{
    std::for_each(m_components.begin(), m_components.end(), [](const auto& component) {
        component->FixedUpdate();
    });
}

void dae::GameObject::Render() const
{
    for (const auto& component : m_components)
    {
        if (auto renderComp = std::dynamic_pointer_cast<RenderComponent>(component))
        {
            renderComp->Render();
        }
    }
}
