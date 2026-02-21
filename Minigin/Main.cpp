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
#include "Components/FPSComponent.h"
#include "Components/RotateAndScaleComponent.h"
#include "Components/OrbitComponent.h"
#include "Scene.h"

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

void CreateLivesRow(
	dae::Scene& scene,
	int lives,
	glm::vec3 startPos,
	float spacing,
	const std::string& spriteFile)
{
	for (int i = 0; i < lives; ++i)
	{
		auto go = std::make_unique<dae::GameObject>();

		go->GetComponent<dae::TransformComponent>()
			->SetLocalPosition({ startPos.x + i * spacing, startPos.y, startPos.z });

		go->AddComponent<dae::TextureComponent>(spriteFile);

		scene.Add(std::move(go));
	}
}

void CreateLevelRow(
	dae::Scene& scene,
	int levelCount,
	glm::vec3 startPos,
	float spacing,
	const std::string& spriteFile)
{
	for (int i = 0; i < levelCount; ++i)
	{
		auto go = std::make_unique<dae::GameObject>();

		go->GetComponent<dae::TransformComponent>()
			->SetLocalPosition({ startPos.x + i * spacing, startPos.y, startPos.z });

		go->AddComponent<dae::TextureComponent>(spriteFile);

		scene.Add(std::move(go));
	}
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

	const std::string spriteFile = "Player.png";

	const int p1Lives = 3;
	const int p2Lives = 3;
	const int level = 1;

	const float startX = 630.f;
	const float spacing = 50.f;

	const float baseY = 380.f;
	const float verticalSpacing = 60.f;

	auto CreateSpriteRow = [&](int count, float yPos)
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
	CreateSpriteRow(p1Lives, baseY);

	//p2 lives
	CreateSpriteRow(p2Lives, baseY + verticalSpacing);

	//current level
	CreateSpriteRow(level, baseY + verticalSpacing * 4);
}

static void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene("Game");
	auto& test = dae::SceneManager::GetInstance().CreateScene("Test");
	test;
	CreateBackground(scene, "Background_Galaga.png");
	CreateHUD(scene);
	
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


