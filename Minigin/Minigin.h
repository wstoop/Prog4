#pragma once
#include <string>
#include <functional>
#include <filesystem>

namespace dae
{
	class Renderer;
	class SceneManager;
	class InputManager;
	class TimeManager;

	class Minigin final
	{
		bool m_quit{};
		std::chrono::time_point<std::chrono::high_resolution_clock> m_lastTime{};
		float m_lag{};
		float m_fixedTimeStep = 1.0f / 60.0f;
		Renderer& m_renderer;
		SceneManager& m_sceneManager;
		InputManager& m_input;
		TimeManager& m_timeManager;

		std::function<void()> m_gameUpdate{};
		std::function<void()> m_gameRender{};
	public:
		explicit Minigin(const std::filesystem::path& dataPath);
		~Minigin();
		void Run(const std::function<void()>& load);
		void RunOneFrame();

		void SetGameUpdate(const std::function<void()>& fn) { m_gameUpdate = fn; }
		void SetGameRender(const std::function<void()>& fn) { m_gameRender = fn; }

		Minigin(const Minigin& other) = delete;
		Minigin(Minigin&& other) = delete;
		Minigin& operator=(const Minigin& other) = delete;
		Minigin& operator=(Minigin&& other) = delete;
	};
}