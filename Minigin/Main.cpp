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
#include "EnemyFactory.h"
#include "Scene.h"
#include <fstream>

#include <filesystem>
namespace fs = std::filesystem;
std::unique_ptr<dae::GameObject> CreateStaticText(
	std::shared_ptr<dae::Font> font,
	SDL_Color color,
	glm::vec3 position,
	const std::string& text)
{
	auto go = std::make_unique<dae::GameObject>();

	go->GetComponent<dae::TransformComponent>()
		->SetLocalPosition(position);

	go->AddComponent<dae::TextComponent>(text, font, color);

	return go;
}

void CreateScoreBlock(
	dae::Scene& scene,
	const std::string& label,
	const std::string& score,
	glm::vec3 labelPos,
	glm::vec3 scorePos,
	std::shared_ptr<dae::Font> font,
	SDL_Color labelColor,
	SDL_Color scoreColor)
{
	scene.Add(CreateStaticText(font, labelColor, labelPos, label));
	scene.Add(CreateStaticText(font, scoreColor, scorePos, score));
}

void CreateBackground(dae::Scene& scene, const std::string& fileName)
{
	auto go = std::make_unique<dae::GameObject>();

	go->GetComponent<dae::TransformComponent>()
		->SetLocalPosition({ 0, 0, 0 });

	go->AddComponent<dae::TextureComponent>(fileName);

	scene.Add(std::move(go));
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
			return enemy;
		});

	dae::EnemyFactory::Register('W', []()
		{
			auto enemy = std::make_unique<dae::GameObject>();
			enemy->AddComponent<dae::TextureComponent>("butterfly.png");
			return enemy;
		});

	dae::EnemyFactory::Register('G', []()
		{
			auto enemy = std::make_unique<dae::GameObject>();
			enemy->AddComponent<dae::TextureComponent>("bird.png");
			return enemy;
		});

	float spacingX{ 45.f };
	float spacingY{ 40.f };

	auto formationData = LoadFormation("./Data/formation1.txt");

	auto formationMover = std::make_unique<dae::GameObject>();
	formationMover->AddComponent<dae::FormationComponent>();
	formationMover->GetComponent<dae::TransformComponent>()
		->SetLocalPosition({ 160.f, 200.f, 0.f });

	auto* formationPtr = formationMover.get();

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

			enemy->GetComponent<dae::TransformComponent>()
				->SetLocalPosition({ col * spacingX, row * spacingY, 0.f });

			enemy->GetComponent<dae::TransformComponent>()
				->SetScale({ 3.f, 3.f, 0.f });

			enemy->GetComponent<dae::TransformComponent>()
				->SetParent(formationPtr, false);

			scene.Add(std::move(enemy));
		}
	}

	scene.Add(std::move(formationMover));
}

static void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene("Game");
	CreateBackground(scene, "Background_Galaga.png");
	CreateHUD(scene);
	CreateEnemies(scene);

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


