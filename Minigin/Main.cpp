#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#include "Minigin.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Components/TransformComponent.h"
#include "Components/TextComponent.h"
#include "Components/TextureComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/FormationComponent.h"
#include "Components/EnemyEntryComponent.h"
#include "Components/EntryQueueComponent.h"
#include "Components/ScrollBackgroundComponent.h"
#include "Components/ThrashCacheComponent.h"
#include "Input/InputManager.h"
#include "Commands/MoveCommand.h"
#include "Commands/ShootCommand.h"
#include "Input/Controller.h"
#include "EnemyFactory.h"
#include "Scene.h"

#include <filesystem>
namespace fs = std::filesystem;

enum class PlayerInputType
{
	Keyboard,
	Thumbstick,
	DPad
};
void CreateBackground(dae::Scene& scene, const std::string& fileName)
{
	auto background = std::make_unique<dae::GameObject>();
	

	auto bgA = std::make_unique<dae::GameObject>();
	bgA->AddComponent<dae::TextureComponent>(fileName);
	bgA->GetComponent<dae::TransformComponent>()->SetParent(background.get(), false);
	auto bgB = std::make_unique<dae::GameObject>();
	bgB->AddComponent<dae::TextureComponent>(fileName);
	bgB->GetComponent<dae::TransformComponent>()->SetParent(background.get(), false);

	background->AddComponent<dae::ScrollBackgroundComponent>(600.f, 830.f);
	scene.Add(std::move(background));
	scene.Add(std::move(bgA));
	scene.Add(std::move(bgB));
}

void CreateHUD(dae::Scene& scene)
{
	auto font = dae::ResourceManager::GetInstance().LoadFont("ArcadeFont.otf", 36);

	SDL_Color redText{ 188, 25, 0, 255 };
	SDL_Color whiteText{ 255, 255, 255, 255 };

	auto CreateText = [&](const std::string& text,
		SDL_Color color,
		glm::vec3 position)
		{
			auto go = std::make_unique<dae::GameObject>();

			go->GetComponent<dae::TransformComponent>()
				->SetLocalPosition(position);

			go->AddComponent<dae::TextComponent>(text, font, color);

			scene.Add(std::move(go));
		};

	//Highscore
	CreateText("HIGH", redText, { 650, 75, 0 });
	CreateText("SCORE", redText, { 670, 95, 0 });
	CreateText("30000", whiteText, { 670, 115, 0 });

	//p1 score
	CreateText("1UP", redText, { 650, 180, 0 });
	CreateText("00", whiteText, { 720, 220, 0 });

	//p2 score
	CreateText("2UP", redText, { 650, 280, 0 });
	CreateText("00", whiteText, { 720, 320, 0 });

	const int p1Lives = 3;
	const int p2Lives = 3;
	const int level = 1;

	const float startX = 630.f;
	const float spacing = 50.f;

	const float baseY = 380.f;
	const float verticalSpacing = 60.f;

	auto CreateSpriteRow = [&](const std::string& spriteFile, int count, float yPos)
		{
			for (int i = 0; i < count; ++i)
			{
				auto go = std::make_unique<dae::GameObject>();

				go->GetComponent<dae::TransformComponent>()
					->SetLocalPosition({ startX + i * spacing, yPos, 0 });

				go->GetComponent<dae::TransformComponent>()
					->SetScale({ 3, 3, 0 });

				go->AddComponent<dae::TextureComponent>(spriteFile);

				scene.Add(std::move(go));
			}
		};

	//p1 lives
	CreateSpriteRow("Player.png", p1Lives, baseY);

	//p2 lives
	CreateSpriteRow("Player.png", p2Lives, baseY + verticalSpacing);

	//current level
	CreateSpriteRow("levelCounter.png", level, baseY + verticalSpacing * 4);
}

void CreateEnemies(dae::Scene& scene)
{
	dae::EnemyFactory::Register('B', []()
		{
			auto enemy = std::make_unique<dae::GameObject>();
			//enemy->AddComponent<dae::TextureComponent>("bee.png");
			enemy->AddComponent<dae::AnimationComponent>("beeIdle.png", 2, 1, 0.2f);
			enemy->GetComponent<dae::TransformComponent>()->SetLocalPosition({ -100.f, -100.f, 0.f });
			return enemy;
		});

	dae::EnemyFactory::Register('W', []()
		{
			auto enemy = std::make_unique<dae::GameObject>();
			//enemy->AddComponent<dae::TextureComponent>("butterfly.png");
			enemy->AddComponent<dae::AnimationComponent>("butterflyIdle.png", 2, 1, 0.2f);
			enemy->GetComponent<dae::TransformComponent>()->SetLocalPosition({ -100.f, -100.f, 0.f });
			return enemy;
		});

	dae::EnemyFactory::Register('G', []()
		{
			auto enemy = std::make_unique<dae::GameObject>();
			//enemy->AddComponent<dae::TextureComponent>("bird.png");
			enemy->AddComponent<dae::AnimationComponent>("birdIdle.png", 2, 2, 0.2f);
			enemy->GetComponent<dae::TransformComponent>()->SetLocalPosition({ -100.f, -100.f, 0.f });
			return enemy;
		});

	float spacingX{ 45.f };
	float spacingY{ 40.f };

	auto formationData = dae::ResourceManager::GetInstance().LoadFormation("formation1.txt");

	auto formationMover = std::make_unique<dae::GameObject>();
	formationMover->GetComponent<dae::TransformComponent>()
		->SetLocalPosition({ 120.f, 200.f, 0.f });
	formationMover->AddComponent<dae::FormationComponent>();
	auto* formationPtr = formationMover.get();
	int enemyCount = 0;
	std::vector<std::vector<dae::EnemyEntryComponent*>> enemiesByCol(formationData[0].size());
	for (size_t row = 0; row < formationData.size(); ++row)
	{
		for (size_t col = 0; col < formationData[row].size(); ++col)
		{
			char cell = formationData[row][col];

			if (cell == '.')
				continue;

			auto enemy = dae::EnemyFactory::Create(cell);
			if (!enemy)
				continue;

			auto* transform = enemy->GetComponent<dae::TransformComponent>();

			glm::vec3 localTarget{ col * spacingX, row * spacingY, 0.f };
			glm::vec3 target{
				formationPtr->GetComponent<dae::TransformComponent>()->GetLocalPosition().x + localTarget.x,
				formationPtr->GetComponent<dae::TransformComponent>()->GetLocalPosition().y + localTarget.y,
				0.f
			};
			transform->SetScale({ 3.f, 3.f, 0.f });

			float delay = 0.05f * row;
			enemy->AddComponent<dae::EnemyEntryComponent>(
				formationPtr,
				transform,
				target,
				2.0f,
				delay
			);

			auto* entryComp = enemy->GetComponent<dae::EnemyEntryComponent>();
			enemiesByCol[col].push_back(entryComp);
			++enemyCount;
			scene.Add(std::move(enemy));
		}
	}

	std::vector<dae::EntryBatch> entryBatches;
	std::vector<std::pair<int, int>> colPairs;
	int totalCols = static_cast<int>(formationData[0].size());
	for (int i = 0; i < totalCols / 2; ++i)
		colPairs.push_back({ i, totalCols - 1 - i });

	bool enterFromLeft = false;
	for (auto& [leftCol, rightCol] : colPairs)
	{
		dae::EntryBatch batch;
		for (auto* entry : enemiesByCol[leftCol])
		{
			entry->SetEntryDirection(enterFromLeft);
			batch.enemies.push_back(entry);
		}
		for (auto* entry : enemiesByCol[rightCol])
		{
			entry->SetEntryDirection(!enterFromLeft);
			batch.enemies.push_back(entry);
		}
		entryBatches.push_back(batch);
	}
	formationMover->AddComponent<dae::EntryQueueComponent>(std::move(entryBatches));
	formationMover->GetComponent<dae::FormationComponent>()->SetAllEnemies(enemyCount);
	scene.Add(std::move(formationMover));
}

void CreatePlayer(dae::Scene& scene, PlayerInputType inputType, float movementSpeed, bool bindInput)
{
	auto& input = dae::InputManager::GetInstance();
	auto player = std::make_unique<dae::GameObject>();
	player->AddComponent<dae::TextureComponent>("Player.png");
	player->AddComponent<dae::ShootComponent>(800.f);
	player->GetComponent<dae::TransformComponent>()
		->SetLocalPosition({ 300.f, 700.f, 0.f });
	player->GetComponent<dae::TransformComponent>()
		->SetScale({ 3.f, 3.f, 0.f });

	if (bindInput)
	{
		uint32_t controllerp1 = UINT32_MAX;
		if (inputType != PlayerInputType::Keyboard)
			controllerp1 = input.AddController();

		switch (inputType)
		{
		case PlayerInputType::Keyboard:
			input.BindCommand(
				{ SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D },
				std::make_unique<MoveCommand>(player.get(), movementSpeed));
			input.BindCommand(SDL_SCANCODE_SPACE, dae::KeyState::Down,
				std::make_unique<ShootCommand>(player.get()));
			break;
		case PlayerInputType::Thumbstick:
			input.BindCommand(controllerp1, dae::Thumbstick::Left,
				std::make_unique<MoveCommand>(player.get(), movementSpeed));
			input.BindCommand(controllerp1, dae::ControllerButton::ButtonA, dae::KeyState::Down,
				std::make_unique<ShootCommand>(player.get()));
			break;
		case PlayerInputType::DPad:
			input.BindCommand(controllerp1, dae::Thumbstick::DPad,
				std::make_unique<MoveCommand>(player.get(), movementSpeed));
			input.BindCommand(controllerp1, dae::ControllerButton::ButtonA, dae::KeyState::Down,
				std::make_unique<ShootCommand>(player.get()));
			break;
		default:
			input.BindCommand(
				{ SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D },
				std::make_unique<MoveCommand>(player.get(), movementSpeed));
			input.BindCommand(SDL_SCANCODE_SPACE, dae::KeyState::Down,
				std::make_unique<ShootCommand>(player.get()));
			break;
		}
	}

	scene.Add(std::move(player));
}

void CreateThrashCache(dae::Scene& scene)
{
	auto go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::ThrashCacheComponent>();
	scene.Add(std::move(go));
}

void CreateMenuScene()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene("Menu");
	CreateBackground(scene, "Background_Galaga.png");
}

void CreateSinglePlayerScene()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene("SinglePlayer");
	CreateBackground(scene, "Background_Galaga.png");
	CreateHUD(scene);
	CreateEnemies(scene);
	CreatePlayer(scene, PlayerInputType::Thumbstick, 100.f, false);
}

void CreateCoOpScene()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene("MultiPlayer");
	CreateBackground(scene, "Background_Galaga.png");
	CreateHUD(scene);
	CreateEnemies(scene);
	CreatePlayer(scene, PlayerInputType::DPad, 200.f, true);
	CreatePlayer(scene, PlayerInputType::Keyboard, 100.f, true);
}

void CreateVersusScene()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene("Versus");
	CreateBackground(scene, "Background_Galaga.png");
	CreateHUD(scene);
	CreateEnemies(scene);
	CreatePlayer(scene, PlayerInputType::Thumbstick, 100.f, false);
}

static void load()
{
	CreateMenuScene();
	CreateSinglePlayerScene();
	CreateCoOpScene();
	CreateVersusScene();
	dae::SceneManager::GetInstance().SetActiveScene("MultiPlayer");
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


