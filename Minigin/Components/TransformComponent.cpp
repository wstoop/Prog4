#include "TransformComponent.h"
#include "../GameObject.h"


void dae::TransformComponent::SetScale(const float x, const float y, const float z)
{
	m_scale.x = x;
	m_scale.y = y;
	m_scale.z = z;
}
void dae::TransformComponent::SetScale(const glm::vec3& scale)
{
	m_scale = scale;
}

void dae::TransformComponent::SetRotation(const float x, const float y, const float z)
{
	m_rotation.x = x;
	m_rotation.y = y;
	m_rotation.z = z;
}
void dae::TransformComponent::SetRotation(const glm::vec3& rotation)
{
	m_rotation = rotation;
}

void dae::TransformComponent::SetParent(GameObject* parent, bool keepWorldPosition)
{
    if (IsChild(parent) || parent == this->GetOwner() || m_parent == parent)
        return;

    auto oldParent = m_parent;
    auto oldParentTransform = oldParent
        ? oldParent->GetComponent<TransformComponent>()
        : nullptr;

    auto newParentTransform = parent
        ? parent->GetComponent<TransformComponent>()
        : nullptr;

    if (parent == nullptr)
    {
        SetLocalPosition(GetWorldPosition());
    }
    else if (keepWorldPosition)
    {
        SetLocalPosition(GetWorldPosition() - newParentTransform->GetWorldPosition());
    }

    if (oldParentTransform)
        oldParentTransform->RemoveChild(this->GetOwner());

    m_parent = parent;

    if (newParentTransform)
        newParentTransform->AddChild(this->GetOwner());

    SetPositionDirty();
}

void dae::TransformComponent::SetLocalPosition(const glm::vec3& pos)
{
    m_localPosition = pos;
    SetPositionDirty();
}

const glm::vec3& dae::TransformComponent::GetWorldPosition()
{
    if (m_positionIsDirty)
        UpdateWorldPosition();
    return m_worldPosition;
}
void dae::TransformComponent::UpdateWorldPosition()
{
    if (m_positionIsDirty)
    {
        if (m_parent == nullptr)
            m_worldPosition = m_localPosition;
        else
            m_worldPosition = m_parent->GetComponent<TransformComponent>()->GetWorldPosition() + m_localPosition;
    }
    m_positionIsDirty = false;
}

void dae::TransformComponent::AddChild(GameObject* child)
{
    m_children.push_back(child); 
}

void dae::TransformComponent::RemoveChild(GameObject* child)
{
    m_children.erase(std::remove_if(m_children.begin(), m_children.end(),
        [child](const GameObject* ptr) { return ptr == child; }), m_children.end());
}

bool dae::TransformComponent::IsChild(GameObject* child) const
{
    return std::find_if(m_children.begin(), m_children.end(),
        [child](const GameObject* ptr) { return ptr == child; }) != m_children.end();
}

void dae::TransformComponent::SetPositionDirty()
{
    m_positionIsDirty = true;
    for (auto child : m_children)
        child->GetComponent<TransformComponent>()->SetPositionDirty();
}

dae::GameObject* dae::TransformComponent::GetParent()
{
    return m_parent;
}