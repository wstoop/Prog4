#include "SelectableButtonComponent.h"
#include "GameObject.h"
#include "Components/TextComponent.h"

dae::SelectableButtonComponent::SelectableButtonComponent(GameObject* owner, ConnectedUIElements elements)
    : Component(owner)
    , m_IsSelected(false)
    , m_ConnectedElements(elements)
{}

void dae::SelectableButtonComponent::Select()
{
    m_IsSelected = true;
	GetOwner()->GetComponent<TextComponent>()->SetColor({ 255, 255, 0 });
}

void dae::SelectableButtonComponent::Deselect()
{
    m_IsSelected = false;
	GetOwner()->GetComponent<TextComponent>()->SetColor({ 255, 255, 255 });
}