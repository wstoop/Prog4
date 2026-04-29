#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#include "Minigin.h"
#include "SceneManager.h"
#include "SceneBuilder.h"
#include "Components/PoolComponent.h"
#include "Sound/ServiceLocator.h"
#include "Sound/SoundSystem.h"
#include "Sound/LoggingSoundSystem.h"
#include "Sound/SDLSoundSystem.h"
#include "SoundID.h"
#include <filesystem>
namespace fs = std::filesystem;

SceneInputBinding CreateMenuScene()
{
    return SceneBuilder("Menu")
        .WithBackground("Background_Galaga.png")
        .Build()
        .inputBinding;
}

SceneInputBinding CreateSinglePlayerScene()
{
    return SceneBuilder("SinglePlayer")
        .WithBackground("Background_Galaga.png")
        .WithEnemies()
        .WithPlayer(PlayerConfig::Keyboard(100.f))
        .WithHUDForPlayer(0)
        .Build()
        .inputBinding;
}

SceneInputBinding CreateCoOpScene()
{
    return SceneBuilder("CoOp")
        .WithBackground("Background_Galaga.png")
        .WithEnemies()
        .WithPlayer(PlayerConfig::Keyboard(100.f, { 200.f, 700.f, 0.f }))
        .WithPlayer(PlayerConfig::Controller(PlayerInputType::DPad, 200.f, { 400.f, 700.f, 0.f }))
        .WithHUDForPlayer(0)
        .WithHUDForPlayer(1)
        .Build()
        .inputBinding;
}

SceneInputBinding CreateVersusScene()
{
    return SceneBuilder("Versus")
        .WithBackground("Background_Galaga.png")
        .WithEnemies()
        .WithPlayer(PlayerConfig::Controller(PlayerInputType::Thumbstick, 100.f))
        .Build()
        .inputBinding;
}

static void load()
{

#ifdef _DEBUG
    dae::ServiceLocator::RegisterSoundSystem(
        std::make_unique<dae::LoggingSoundSystem>(
            std::make_unique<dae::SDLSoundSystem>()));
#else
    dae::ServiceLocator::RegisterSoundSystem(
        std::make_unique<dae::SDLSoundSystem>());
#endif

    auto& ss = dae::ServiceLocator::GetSoundSystem();
    ss.RegisterSound(SOUND_SHOOT, "Player_Shoot.mp3");
    ss.RegisterSound(SOUND_ENEMY_HIT, "Enemy_Hit.mp3");
    ss.RegisterSound(SOUND_BOSS_HIT_1, "Boss_Hit_Once.mp3");
    ss.RegisterSound(SOUND_BOSS_HIT_2, "Boss_Hit_Twice.mp3");

    auto menuBinding = CreateMenuScene();
    auto singleBinding = CreateSinglePlayerScene();
    auto coopBinding = CreateCoOpScene();
    auto versusBinding = CreateVersusScene();

    coopBinding.SwitchFrom(singleBinding);
    dae::SceneManager::GetInstance().SetActiveScene("CoOp");

    auto poolGO = std::make_unique<dae::GameObject>();
    poolGO->AddComponent<dae::PoolComponent>();
    dae::SceneManager::GetInstance().GetActiveScene()->Add(std::move(poolGO));
}

int main(int, char*[]) {
#if __EMSCRIPTEN__
	fs::path data_location = "";
#else
	fs::path data_location = "./Data/";
	if(!fs::exists(data_location))
		data_location = "../Data/";
#endif
	dae::Minigin engine(data_location);
    engine.Run(load);
    return 0;
}


