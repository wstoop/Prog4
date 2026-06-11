#pragma once
#include "Commands/Command.h"
#include "GameObject.h"
#include "Components/SelectableButtonComponent.h"
#include "TimeManager.h"
#include "UISelection.h"
#include <glm/glm.hpp>

class UIMoveCommand : public AxisCommand
{
public:
    UIMoveCommand(UISelection* selection)
        : m_pSelection(selection)
    {}

    void Execute(glm::vec2 axis) override
    {
        // 1. Ensure safety checks work dynamically
        if (!m_pSelection || !m_pSelection->current) return;

        auto* buttonComp = m_pSelection->current->GetComponent<dae::SelectableButtonComponent>();
        if (!buttonComp) return;

        auto elements = buttonComp->GetConnectedElements();
        dae::GameObject* nextButton = nullptr;

        // 2. Map input layout directions
        if (axis.y > 0)      nextButton = elements.m_pUpButton;
        else if (axis.y < 0) nextButton = elements.m_pDownButton;
        else if (axis.x < 0) nextButton = elements.m_pLeftButton;
        else if (axis.x > 0) nextButton = elements.m_pRightButton;

        if (!nextButton) return;

        auto* nextComp = nextButton->GetComponent<dae::SelectableButtonComponent>();
        if (!nextComp) return;

        // 3. Swap visual elements and tracking states
        buttonComp->Deselect();
        nextComp->Select();
        m_pSelection->current = nextButton;
    }
private:
    UISelection* m_pSelection;
};