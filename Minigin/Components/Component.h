#pragma once

namespace dae
{
    class GameObject;

    class Component
    {
		friend GameObject;
        GameObject* m_owner{ nullptr };
    public:
        virtual ~Component() = default;

        virtual void Update() {}
        virtual void LateUpdate() {}
        virtual void FixedUpdate() {}

        
        GameObject* GetOwner() const { return m_owner; }
        Component(GameObject* owner) { m_owner = owner; }
        Component(const Component& other) = delete;
        Component(Component&& other) = delete;
        Component& operator=(const Component& other) = delete;
        Component& operator=(Component&& other) = delete;
    };
}
