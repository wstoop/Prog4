#include "EntryQueueComponent.h"
#include "EnemyEntryComponent.h"
#include "../GameObject.h"
#include <algorithm>

dae::EntryQueueComponent::EntryQueueComponent(GameObject* owner, std::vector<dae::EntryBatch> batches)
    : Component(owner)
    , m_batches(std::move(batches))
{
}

void dae::EntryQueueComponent::Update()
{
    if (m_currentBatch >= static_cast<int>(m_batches.size()))
        return;

    if (!m_batchStarted)
    {
        std::for_each(
            m_batches[m_currentBatch].enemies.begin(),
            m_batches[m_currentBatch].enemies.end(),
            [](auto* entry)
            {
                entry->StartEntryTop(entry->GetFromLeft());
            });
        m_batchStarted = true;
    }

    bool allDone = std::all_of(
        m_batches[m_currentBatch].enemies.begin(),
        m_batches[m_currentBatch].enemies.end(),
        [](EnemyEntryComponent* e) { return e->IsDocked() || e->GetOwner()->m_destroy; }
    );

    if (allDone)
    {
        ++m_currentBatch;
        m_batchStarted = false;
    }
}