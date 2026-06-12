#pragma once
#include "Commands/Command.h"
#include "Components/EnemyHealthComponent.h"
#include "GameObject.h"
#include "Scene.h"
#include "SceneManager.h"

class SkipWaveCommand : public Command
{
public:
    void Execute() override
    {
        auto* scene = dae::SceneManager::GetInstance().GetActiveScene();
        if (!scene) return;

        for (const auto& obj : scene->GetObjects())
        {
            if (obj->m_destroy || obj->tag != "Enemy") continue;

            if (auto* health = obj->GetComponent<dae::EnemyHealthComponent>())
                health->Kill();
        }
    }
};
