#pragma once
#include "Singleton.h"
#include "Controller.h"
#include "../Commands/Command.h"
#include <memory>
#include <glm/glm.hpp>
#include <map>
namespace dae
{
	enum class KeyState
	{
		Down,
		Up,
		Pressed
	};
	enum class Thumbstick { Left, Right, DPad };
	class InputManager final : public Singleton<InputManager>
	{
	public:
		using ControllerKey = std::pair<uint32_t, Controller::Button>;
		using KeyboardKey = std::pair<uint32_t, KeyState>;
		using ThumbstickKey = std::pair<uint32_t, Thumbstick>;

		void AddController(uint32_t controllerIndex);

		void BindCommand(ControllerKey controllerKey, KeyState keyState, std::unique_ptr<Command> command);
		void BindCommand(KeyboardKey keyboardKey, std::unique_ptr<Command> command);
		void BindCommand(ThumbstickKey thumbstickKey, std::unique_ptr<AxisCommand> command);

		void UnbindCommand(ThumbstickKey thumbstickKey);
		void UnbindCommand(ControllerKey controllerKey, KeyState keyState);
		void UnbindCommand(KeyboardKey keyboardKey);

		bool ProcessInput();
	private:
		void ScanControllers();

		std::map<uint32_t, std::unique_ptr<Controller>> m_controllers;

		std::map<std::pair<ControllerKey, KeyState>, std::unique_ptr<Command>> m_controllerCommands;
		std::map<KeyboardKey, std::unique_ptr<Command>> m_keyboardCommands;
		std::map<ThumbstickKey, std::unique_ptr<AxisCommand>> m_thumbstickCommands;
	};

}
