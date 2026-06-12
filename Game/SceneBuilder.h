#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <glm/glm.hpp>
#include "Commands/UISelection.h"
#include "Components/GameEvents.h"
#include "Input/InputManager.h"

namespace dae { class Scene; class GameObject; }
//Input types
enum class PlayerInputType { Keyboard, Thumbstick, DPad };
//Geenral player stats
struct PlayerConfig
{
    PlayerInputType inputType = PlayerInputType::Keyboard;
    float speed = 100.f;
    glm::vec3 spawnPos = { 300.f, 700.f, 0.f };
    int lives = 4;
    float maxHp = 1.f;

    static PlayerConfig Keyboard(float speed = 100.f, glm::vec3 pos = { 300.f, 700.f, 0.f })
    {
        return { PlayerInputType::Keyboard, speed, pos };
    }
    static PlayerConfig Controller(PlayerInputType stick, float speed = 100.f,
        glm::vec3 pos = { 500.f, 700.f, 0.f })
    {
        return { stick, speed, pos };
    }
};

//stores input and commands per scene
struct SceneInputBinding
{
    struct PlayerBinding
    {
        dae::GameObject* playerPtr = nullptr;
        PlayerConfig     config;
        uint32_t         controllerID = UINT32_MAX;
        bool             controlsBoss = false;
    };

    std::vector<PlayerBinding> players;
    std::vector<std::unique_ptr<UISelection>> ownedSelections;

    std::vector<std::function<void()>> onBind;
    std::vector<std::function<void()>> onUnbind;

    // Bind the commands in this scene
    void Bind();

    // Removes commands in the current scene
    void Unbind();

    //Unbind previous inputs, binds the next ones
    void SwitchFrom(SceneInputBinding& previous);
};

//result after making a scene, gives amount of players and all the command bindings
struct BuildResult
{
    std::vector<dae::GameObject*> playerPtrs;
    SceneInputBinding inputBinding;
    dae::GameObject* menuStartingButton{ nullptr };
    dae::GameObject* bossPtr{ nullptr };
};


class SceneBuilder
{
public:
    //makes a new scene
    explicit SceneBuilder(const std::string& sceneName);

    //used to add stuff to the scene
    SceneBuilder& WithBackground(const std::string& textureFile,
        float screenW = 600.f, float screenH = 830.f);
    SceneBuilder& WithEnemies(const std::string& formationFile = "formation1.txt");
    SceneBuilder& WithPlayerControlledBoss();
    SceneBuilder& WithPlayer(const PlayerConfig& cfg);
    SceneBuilder& WithHUDForPlayer(int playerIndex);
    SceneBuilder& WithMenuButtons();
    SceneBuilder& WithNextButton(EventId nextStateEventId);

    BuildResult Build();
    static void ActivateSceneWithPools(const std::string& sceneName);
private:
    //All of these are helper functions to add required things to a scene
    void SpawnBackground(dae::Scene& scene) const;
    void SpawnEnemies(dae::Scene& scene) const;
    void SpawnPlayerControlledBoss(dae::Scene& scene) const;
    dae::GameObject* SpawnPlayer(dae::Scene& scene, const PlayerConfig& cfg) const;
    void SpawnMenuButtons(dae::Scene& scene, SceneInputBinding& binding);
    void SpawnHUD(dae::Scene& scene, dae::GameObject* playerPtr, int playerIndex) const;

    void SpawnNextButton(dae::Scene& scene, SceneInputBinding& binding);

    std::string m_sceneName;
    std::string m_backgroundFile;
    float m_screenW = 600.f;
    float m_screenH = 830.f;
    bool m_hasBackground = false;
    bool m_hasEnemies = false;
    std::string m_formationFile = "formation1.txt";
    std::vector<PlayerConfig> m_players;
    std::vector<int> m_hudPlayerIndices;

    bool m_playerControlledBoss = false;
    mutable dae::GameObject* m_pBossPtr{ nullptr };

    bool m_hasMenuButtons = false;
    bool m_hasNextButton = false;
    EventId m_nextButtonEventId = 0;
    dae::GameObject* m_pMenuStartingButton{ nullptr };
};