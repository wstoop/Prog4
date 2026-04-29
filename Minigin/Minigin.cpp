#include <stdexcept>
#include <sstream>
#include <iostream>
#include <thread>

#if WIN32
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#endif

#if USE_STEAMWORKS
#pragma warning (push)
#pragma warning (disable:4996)
#include <steam_api.h>
#include "Steam/Achievement.h"
#pragma warning (pop)
#endif

#include <SDL3/SDL.h>
//#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "Minigin.h"
#include "Input/InputManager.h"
#include "SceneManager.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "TimeManager.h"
#include "GameInfo.h"

SDL_Window* g_window{};

void LogSDLVersion(const std::string& message, int major, int minor, int patch)
{
#if WIN32
	std::stringstream ss;
	ss << message << major << "." << minor << "." << patch << "\n";
	OutputDebugString(ss.str().c_str());
#else
	std::cout << message << major << "." << minor << "." << patch << "\n";
#endif
}

#ifdef __EMSCRIPTEN__
#include "emscripten.h"

void LoopCallback(void* arg)
{
	static_cast<dae::Minigin*>(arg)->RunOneFrame();
}
#endif

// Why bother with this? Because sometimes students have a different SDL version installed on their pc.
// That is not a problem unless for some reason the dll's from this project are not copied next to the exe.
// These entries in the debug output help to identify that issue.
void PrintSDLVersion()
{
	LogSDLVersion("Compiled with SDL", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION);
	int version = SDL_GetVersion();
	LogSDLVersion("Linked with SDL ", SDL_VERSIONNUM_MAJOR(version), SDL_VERSIONNUM_MINOR(version), SDL_VERSIONNUM_MICRO(version));
	// LogSDLVersion("Compiled with SDL_image ",SDL_IMAGE_MAJOR_VERSION, SDL_IMAGE_MINOR_VERSION, SDL_IMAGE_MICRO_VERSION);
	// version = IMG_Version();
	// LogSDLVersion("Linked with SDL_image ", SDL_VERSIONNUM_MAJOR(version), SDL_VERSIONNUM_MINOR(version), SDL_VERSIONNUM_MICRO(version));
	LogSDLVersion("Compiled with SDL_ttf ",	SDL_TTF_MAJOR_VERSION, SDL_TTF_MINOR_VERSION,SDL_TTF_MICRO_VERSION);
	version = TTF_Version();
	LogSDLVersion("Linked with SDL_ttf ", SDL_VERSIONNUM_MAJOR(version), SDL_VERSIONNUM_MINOR(version),	SDL_VERSIONNUM_MICRO(version));
}

dae::Minigin::Minigin(const std::filesystem::path& dataPath)
	: m_renderer(Renderer::GetInstance())
	, m_sceneManager(SceneManager::GetInstance())
	, m_input(InputManager::GetInstance())
	, m_timeManager(TimeManager::GetInstance())
{

	PrintSDLVersion();
	
	if (!SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_AUDIO))
	{
		SDL_Log("Renderer error: %s", SDL_GetError());
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
	}
	int totalWidth = 800;
	int height = 830;
	dae::GameInfo::GetInstance().SetScreenDimensions(totalWidth, height);
	dae::GameInfo::GetInstance().SetGameScreenWidth(800 - 150);

#if USE_STEAMWORKS
	if (!SteamAPI_Init())
		throw std::runtime_error(std::string("Fatal Error - Steam must be running to play this game (SteamAPI_Init() failed)."));
	g_SteamAchievements = new CSteamAchievements(g_Achievements, 4);
#endif
	g_window = SDL_CreateWindow(
		"Galaga - Warre Stoop",
		totalWidth,
		height,
		SDL_WINDOW_OPENGL
	);
	if (g_window == nullptr) 
	{
		throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
	}

	Renderer::GetInstance().Init(g_window);
	ResourceManager::GetInstance().Init(dataPath);


}

dae::Minigin::~Minigin()
{
	Renderer::GetInstance().Destroy();
	SDL_DestroyWindow(g_window);
	g_window = nullptr;
	SDL_Quit();

#if USE_STEAMWORKS
	SteamAPI_Shutdown();
	g_SteamAchievements = nullptr;
#endif

}

void dae::Minigin::Run(const std::function<void()>& load)
{
	load();
	m_lastTime = std::chrono::high_resolution_clock::now();
#ifndef __EMSCRIPTEN__
	while (!m_quit)
		RunOneFrame();
#else
	emscripten_set_main_loop_arg(&LoopCallback, this, 0, true);
#endif
}

void dae::Minigin::RunOneFrame()
{
	const auto current_time = std::chrono::high_resolution_clock::now();
	const float delta_time = std::chrono::duration<float>(current_time - m_lastTime).count();
	m_quit = !m_input.ProcessInput();
#if USE_STEAMWORKS
	SteamAPI_RunCallbacks();
#endif 

	m_timeManager.SetDeltaTime(delta_time);
	m_lastTime = current_time;
	m_lag += delta_time;

	while (m_lag >= m_fixedTimeStep)
	{
		m_sceneManager.FixedUpdate();
		m_lag -= m_fixedTimeStep;
	}
	m_sceneManager.Update();
	m_sceneManager.LateUpdate();
	m_renderer.Render();
	constexpr int ms_per_frame = 1000 / 120;
	const auto sleep_time = current_time + std::chrono::milliseconds(ms_per_frame) - std::chrono::high_resolution_clock::now();
	std::this_thread::sleep_for(sleep_time);
}
