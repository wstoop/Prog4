#pragma once

namespace dae
{
    class GameObject;

    class Component
    {
		friend GameObject;
        void SetOwner(GameObject* owner) { m_owner = owner; }
    public:
        virtual ~Component() = default;

        virtual void Update() {}
        virtual void LateUpdate() {}
        virtual void FixedUpdate() {}

        
        GameObject* GetOwner() const { return m_owner; }

    protected:
        GameObject* m_owner{ nullptr };

    };
}
