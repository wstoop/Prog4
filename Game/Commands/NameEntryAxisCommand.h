#pragma once
#include "Commands/Command.h"
#include "Components/NameEntryComponent.h"
#include <glm/glm.hpp>

class NameEntryAxisCommand : public AxisCommand
{
public:
    explicit NameEntryAxisCommand(dae::NameEntryComponent* entry)
        : m_pEntry(entry)
    {}

    void Execute(glm::vec2 axis) override
    {
        if (!m_pEntry) return;

        if (axis.x > 0)      m_pEntry->MoveSlot(1);
        else if (axis.x < 0) m_pEntry->MoveSlot(-1);
        else if (axis.y > 0) m_pEntry->CycleLetter(1);
        else if (axis.y < 0) m_pEntry->CycleLetter(-1);
    }
private:
    dae::NameEntryComponent* m_pEntry;
};
