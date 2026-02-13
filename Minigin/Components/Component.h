#pragma once

namespace dae
{
    class GameObject;

    class Component
    {
    public:
        virtual ~Component() = default;

        virtual void Update() {}
        virtual void LateUpdate() {}
        virtual void FixedUpdate() {}
        virtual void Render() const {}

        void SetOwner(GameObject* owner) { m_owner = owner; }
        GameObject* GetOwner() const { return m_owner; }

    protected:
        GameObject* m_owner{ nullptr };
    };
}
