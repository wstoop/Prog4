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

static void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();
	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
	auto color = SDL_Color{ 255, 255, 0, 255 };

	auto go = std::make_unique<dae::GameObject>();
	go->GetComponent<dae::TransformComponent>()->SetLocalPosition(glm::vec3{ 0, 0, 0});
	go->AddComponent<dae::TextureComponent>(go, "background.png");
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->GetComponent<dae::TransformComponent>()->SetLocalPosition(glm::vec3{ 358, 180, 0 });
	go->AddComponent<dae::RotateAndScaleComponent>(go);
	go->AddComponent<dae::TextureComponent>(go, "logo.png");
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->GetComponent<dae::TransformComponent>()->SetLocalPosition(glm::vec3{ 292, 20, 0 });
	go->AddComponent<dae::TextComponent>(go, "Programming 4 Assignment", font, color);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->GetComponent<dae::TransformComponent>()->SetLocalPosition(glm::vec3{ 450, 300, 0 });
	go->AddComponent<dae::FPSComponent>(go);
	go->AddComponent<dae::TextComponent>(go, "FPS", font, color);
	scene.Add(std::move(go));

	auto go0 = std::make_unique<dae::GameObject>();
	auto go1 = std::make_unique<dae::GameObject>();
	auto go2 = std::make_unique<dae::GameObject>();
	go0->GetComponent<dae::TransformComponent>()->SetLocalPosition(glm::vec3{ 100, 100, 0 });
	go0->AddComponent<dae::TextureComponent>(go0, "Player.png");
	go0->AddComponent<dae::RotateAndScaleComponent>(go0, -90.0f, 0.5f, 0.5f, 1.5f);

	go1->GetComponent<dae::TransformComponent>()->SetParent(go0.get(), false);
	go1->AddComponent<dae::OrbitComponent>(go1, 50.f, 1.f, 0.f);
	go1->AddComponent<dae::RotateAndScaleComponent>(go1);
	go1->AddComponent<dae::TextureComponent>(go1, "Player.png");

	go2->GetComponent<dae::TransformComponent>()->SetParent(go1.get(), false);
	go2->AddComponent<dae::OrbitComponent>(go2, 70.f, -1.f, 0.f);
	go2->AddComponent<dae::RotateAndScaleComponent>(go2);
	go2->AddComponent<dae::TextureComponent>(go2, "Player.png");
	scene.Add(std::move(go0));
	scene.Add(std::move(go1));
	scene.Add(std::move(go2));
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
