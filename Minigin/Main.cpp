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
#include "Components/FormationComponent.h"
#include "Components/EnemyEntryComponent.h"
#include "Components/EntryQueueComponent.h"
#include "Components/OrbitComponent.h"
#include "Components/ScrollBackgroundComponent.h"
#include "EnemyFactory.h"
#include "Scene.h"
#include <fstream>

#include <filesystem>
namespace fs = std::filesystem;

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

std::vector<std::string> LoadFormation(const std::string& path)
{
	std::vector<std::string> lines;
	std::ifstream file(path);

	if (!file.is_open())
	{
		throw std::runtime_error(std::string("No formation file found"));
	}
	std::string line;
	while (std::getline(file, line))
	{
		lines.push_back(line);
	}

	return lines;
}

void CreateEnemies(dae::Scene& scene)
{
	dae::EnemyFactory::Register('B', []()
		{
			auto enemy = std::make_unique<dae::GameObject>();
			enemy->AddComponent<dae::TextureComponent>("bee.png");
			enemy->GetComponent<dae::TransformComponent>()->SetLocalPosition({ -100.f, -100.f, 0.f });
			return enemy;
		});

	dae::EnemyFactory::Register('W', []()
		{
			auto enemy = std::make_unique<dae::GameObject>();
			enemy->AddComponent<dae::TextureComponent>("butterfly.png");
			enemy->GetComponent<dae::TransformComponent>()->SetLocalPosition({ -100.f, -100.f, 0.f });
			return enemy;
		});

	dae::EnemyFactory::Register('G', []()
		{
			auto enemy = std::make_unique<dae::GameObject>();
			enemy->AddComponent<dae::TextureComponent>("bird.png");
			enemy->GetComponent<dae::TransformComponent>()->SetLocalPosition({ -100.f, -100.f, 0.f });
			return enemy;
		});

	float spacingX{ 45.f };
	float spacingY{ 40.f };

	auto formationData = LoadFormation("./Data/formation1.txt");

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

			float delay = (row * 0.05f) + (col * 0.05f);
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

	bool enterFromLeft = true;
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
		enterFromLeft = !enterFromLeft;
	}
	formationMover->AddComponent<dae::EntryQueueComponent>(std::move(entryBatches));
	formationMover->GetComponent<dae::FormationComponent>()->SetAllEnemies(enemyCount);
	scene.Add(std::move(formationMover));
}

void CreatePlayer(dae::Scene& scene)
{
	auto empty = std::make_unique<dae::GameObject>();
	empty->GetComponent<dae::TransformComponent>()
		->SetLocalPosition({ 300.f, 700.f, 0.f });

	auto player = std::make_unique<dae::GameObject>();
	player->AddComponent<dae::TextureComponent>("Player.png");
	player->GetComponent<dae::TransformComponent>()
		->SetLocalPosition({ 300.f, 700.f, 0.f });
	player->GetComponent<dae::TransformComponent>()
		->SetScale({ 3.f, 3.f, 0.f });
	player->AddComponent<dae::OrbitComponent>(50.f, -2.f, 0.f);
	player->GetComponent<dae::TransformComponent>()->SetParent(empty.get(), true);

	auto player1 = std::make_unique<dae::GameObject>();
	player1->AddComponent<dae::TextureComponent>("Player.png");
	player1->GetComponent<dae::TransformComponent>()
		->SetLocalPosition({ 300.f, 700.f, 0.f });
	player1->GetComponent<dae::TransformComponent>()
		->SetScale({ 3.f, 3.f, 0.f });
	player1->GetComponent<dae::TransformComponent>()->SetParent(player.get(), true);
	player1->AddComponent<dae::OrbitComponent>(50.f, 2.f, 0.f);
	scene.Add(std::move(empty));
	scene.Add(std::move(player));
	scene.Add(std::move(player1));
}

static void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene("Game");
	CreateBackground(scene, "Background_Galaga.png");
	CreateHUD(scene);
	CreateEnemies(scene);
	CreatePlayer(scene);

	dae::SceneManager::GetInstance().SetActiveScene("Game");
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


