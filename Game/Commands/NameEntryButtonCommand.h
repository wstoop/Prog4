#pragma once
#include "Commands/Command.h"
#include "Components/NameEntryComponent.h"

class NameEntryButtonCommand : public Command
{
public:
    enum class Action { SlotLeft, SlotRight, LetterUp, LetterDown };

    NameEntryButtonCommand(dae::NameEntryComponent* entry, Action action)
        : m_pEntry(entry)
        , m_action(action)
    {}

    void Execute() override
    {
        if (!m_pEntry) return;

        switch (m_action)
        {
        case Action::SlotLeft:   m_pEntry->MoveSlot(-1); break;
        case Action::SlotRight:  m_pEntry->MoveSlot(1);  break;
        case Action::LetterUp:   m_pEntry->CycleLetter(1);  break;
        case Action::LetterDown: m_pEntry->CycleLetter(-1); break;
        }
    }
private:
    dae::NameEntryComponent* m_pEntry;
    Action m_action;
};
