#pragma once
#include "Commands/Command.h"
#include "Components/NameEntryComponent.h"
#include "Components/HighScoreManager.h"
#include <functional>

class NameEntryConfirmCommand : public Command
{
public:
    NameEntryConfirmCommand(dae::NameEntryComponent* entry, int score, std::function<void()> onDone)
        : m_pEntry(entry)
        , m_score(score)
        , m_onDone(std::move(onDone))
    {}

    void Execute() override
    {
        if (!m_pEntry) return;
        dae::HighScoreManager::Insert(m_pEntry->GetName(), m_score);
        if (m_onDone) m_onDone();
    }
private:
    dae::NameEntryComponent* m_pEntry;
    int m_score;
    std::function<void()> m_onDone;
};
