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
		std::chrono::time_point<std::chrono::high_resolution_clock> m_LastTime{};
		float m_Lag{};
		float m_FixedTimeStep = 1.0f / 60.0f;
		Renderer& m_Renderer;
		SceneManager& m_SceneManager;
		InputManager& m_Input;
		TimeManager& m_TimeManager;
	public:
		explicit Minigin(const std::filesystem::path& dataPath);
		~Minigin();
		void Run(const std::function<void()>& load);
		void RunOneFrame();

		Minigin(const Minigin& other) = delete;
		Minigin(Minigin&& other) = delete;
		Minigin& operator=(const Minigin& other) = delete;
		Minigin& operator=(Minigin&& other) = delete;
	};
}