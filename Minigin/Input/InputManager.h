#pragma once
#include "Singleton.h"
#include "../Commands/Command.h"
#include <memory>
#include <cstdint>

namespace dae
{
	enum class KeyState { Down, Up, Pressed };
	enum class Thumbstick { Left, Right, DPad };

	enum class ControllerButton : uint16_t
	{
		DpadUp = 0x0001,
		DpadDown = 0x0002,
		DpadLeft = 0x0004,
		DpadRight = 0x0008,
		Start = 0x0010,
		Back = 0x0020,
		LeftThumb = 0x0040,
		RightThumb = 0x0080,
		LeftShoulder = 0x0100,
		RightShoulder = 0x0200,
		ButtonA = 0x1000,
		ButtonB = 0x2000,
		ButtonX = 0x4000,
		ButtonY = 0x8000,
	};

	// Use int so this header stays SDL-free. Pass SDL_SCANCODE_* values directly —
	// they implicitly convert to int.
	struct KeyboardAxis
	{
		int scancodeUp;
		int scancodeDown;
		int scancodeLeft;
		int scancodeRight;

		bool operator<(const KeyboardAxis& o) const
		{
			if (scancodeUp != o.scancodeUp)    return scancodeUp < o.scancodeUp;
			if (scancodeDown != o.scancodeDown)  return scancodeDown < o.scancodeDown;
			if (scancodeLeft != o.scancodeLeft)  return scancodeLeft < o.scancodeLeft;
			return scancodeRight < o.scancodeRight;
		}
	};

	class InputManager final : public Singleton<InputManager>
	{
	public:
		InputManager();
		~InputManager();

		uint32_t AddController();

		// Controller buttons
		void BindCommand(uint32_t controllerIndex, ControllerButton button, KeyState state,
			std::unique_ptr<Command> command);
		void UnbindCommand(uint32_t controllerIndex, ControllerButton button, KeyState state);

		// Thumbsticks and DPad
		void BindCommand(uint32_t controllerIndex, Thumbstick thumbstick,
			std::unique_ptr<AxisCommand> command);
		void UnbindCommand(uint32_t controllerIndex, Thumbstick thumbstick);

		// Keyboard single key
		void BindCommand(int scancode, KeyState state, std::unique_ptr<Command> command);
		void UnbindCommand(int scancode, KeyState state);

		// Keyboard axis (WASD / arrow keys)
		void BindCommand(KeyboardAxis axis, KeyState state, std::unique_ptr<AxisCommand> command);
		void UnbindCommand(KeyboardAxis axis, KeyState state);

		bool ProcessInput();
	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}