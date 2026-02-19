#include <algorithm>
#include "GameObject.h"
#include "Components/Component.h"
#include "Components/RenderComponent.h"
#include "Components/TransformComponent.h"

dae::GameObject::GameObject()
{
    AddComponent<TransformComponent>();
}
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
    std::for_each(m_components.begin(), m_components.end(),
        [](const std::unique_ptr<Component>& component)
        {
            if (auto renderComp = dynamic_cast<RenderComponent*>(component.get()))
            {
                renderComp->Render();
            }
        });
}
