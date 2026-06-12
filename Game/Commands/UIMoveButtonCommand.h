#pragma once
#include "Commands/Command.h"
#include "GameObject.h"
#include "Components/SelectableButtonComponent.h"
#include "UISelection.h"
#include <glm/glm.hpp>

// Same navigation logic as UIMoveCommand, but triggered by a discrete
// button press (e.g. a D-pad direction) instead of a continuous axis.
class UIMoveButtonCommand : public Command
{
public:
    UIMoveButtonCommand(UISelection* selection, glm::vec2 direction)
        : m_pSelection(selection)
        , m_direction(direction)
    {}

    void Execute() override
    {
        if (!m_pSelection || !m_pSelection->current) return;

        auto* buttonComp = m_pSelection->current->GetComponent<dae::SelectableButtonComponent>();
        if (!buttonComp) return;

        auto elements = buttonComp->GetConnectedElements();
        dae::GameObject* nextButton = nullptr;

        if (m_direction.y > 0)      nextButton = elements.m_pUpButton;
        else if (m_direction.y < 0) nextButton = elements.m_pDownButton;
        else if (m_direction.x < 0) nextButton = elements.m_pLeftButton;
        else if (m_direction.x > 0) nextButton = elements.m_pRightButton;

        if (!nextButton) return;

        auto* nextComp = nextButton->GetComponent<dae::SelectableButtonComponent>();
        if (!nextComp) return;

        buttonComp->Deselect();
        nextComp->Select();
        m_pSelection->current = nextButton;
    }
private:
    UISelection* m_pSelection;
    glm::vec2 m_direction;
};
