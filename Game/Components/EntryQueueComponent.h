#pragma once
#include "Components/Component.h"
#include <vector>

namespace dae
{
    class EnemyEntryComponent;

    struct EntryBatch
    {
        std::vector<EnemyEntryComponent*> enemies;
    };

    class EntryQueueComponent : public Component
    {
    public:
        EntryQueueComponent(GameObject* owner, std::vector<dae::EntryBatch> batches);
        void Update() override;

    private:
        std::vector<EntryBatch> m_batches;
        int m_currentBatch{ 0 };
        bool m_batchStarted{ false };
    };
}