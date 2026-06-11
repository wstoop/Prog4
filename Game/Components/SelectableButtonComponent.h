#pragma once
#include "Components/Component.h"
#include <functional>

namespace dae
{
    class SelectableButtonComponent final : public Component
    {
    public:
		struct ConnectedUIElements
		{
			GameObject* m_pUpButton{ nullptr };
			GameObject* m_pDownButton{ nullptr };
			GameObject* m_pLeftButton{ nullptr };
			GameObject* m_pRightButton{ nullptr };
		};

        SelectableButtonComponent(GameObject* owner, ConnectedUIElements elements);

        void Select();
        void Deselect();
        bool IsSelected() const { return m_IsSelected; }

        void SetOnActivate(std::function<void()> callback) { m_onActivate = callback; }
        void Activate() { if (m_onActivate) m_onActivate(); }

        ConnectedUIElements GetConnectedElements() const { return m_ConnectedElements; }


    private:
        std::function<void()> m_onActivate;
        bool m_IsSelected;
        ConnectedUIElements m_ConnectedElements;
    };
}