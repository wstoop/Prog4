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
	go->AddComponent<dae::TextureComponent>("background.png");
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->GetComponent<dae::TransformComponent>()->SetLocalPosition(glm::vec3{ 358, 180, 0 });
	go->AddComponent<dae::RotateAndScaleComponent>();
	go->AddComponent<dae::TextureComponent>("logo.png");
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->GetComponent<dae::TransformComponent>()->SetLocalPosition(glm::vec3{ 292, 20, 0 });
	go->AddComponent<dae::TextComponent>("Programming 4 Assignment", font, color);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->GetComponent<dae::TransformComponent>()->SetLocalPosition(glm::vec3{ 450, 300, 0 });
	go->AddComponent<dae::FPSComponent>();
	go->AddComponent<dae::TextComponent>("FPS", font, color);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	auto go1 = std::make_unique<dae::GameObject>();
	go1->GetComponent<dae::TransformComponent>()->SetLocalPosition(glm::vec3{ 100, 100, 0 });
	go1->AddComponent<dae::TextureComponent>("Player.png");
	go->GetComponent<dae::TransformComponent>()->SetLocalPosition(glm::vec3{ 200, 200, 0 });
	go->GetComponent<dae::TransformComponent>()->SetParent(go1.get(), false);
	go->AddComponent<dae::TextureComponent>("Player.png");
	scene.Add(std::move(go1));
	scene.Add(std::move(go));
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
