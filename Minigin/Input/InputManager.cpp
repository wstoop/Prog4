#include <SDL3/SDL.h>
#include <backends/imgui_impl_sdl3.h>
#include "InputManager.h"
#include <algorithm>


void dae::InputManager::AddController(uint32_t controllerIndex)
{
	m_controllers.emplace(controllerIndex, std::make_unique<Controller>(controllerIndex));
}

void dae::InputManager::BindCommand(ControllerKey controllerKey, KeyState keyState, std::unique_ptr<Command> command)
{
	m_controllerCommands[{controllerKey, keyState}] = std::move(command);
}

void dae::InputManager::BindCommand(KeyboardKey keyboardKey, std::unique_ptr<Command> command)
{
	m_keyboardCommands[keyboardKey] = std::move(command);
}

void dae::InputManager::BindCommand(ThumbstickKey thumbstickKey, std::unique_ptr<AxisCommand> command)
{
    m_thumbstickCommands[thumbstickKey] = std::move(command);
}

void dae::InputManager::UnbindCommand(ThumbstickKey thumbstickKey)
{
    m_thumbstickCommands.erase(thumbstickKey);
}

void dae::InputManager::UnbindCommand(ControllerKey controllerKey, KeyState keyState)
{
	m_controllerCommands.erase({ controllerKey, keyState });
}

void dae::InputManager::UnbindCommand(KeyboardKey keyboardKey)
{
	m_keyboardCommands.erase(keyboardKey);

}

void dae::InputManager::ScanControllers()
{
    for (uint32_t i = 0; i < 4; ++i) // max 4 controllers
    {
        if (m_controllers.find(i) == m_controllers.end())
        {
            auto controller = std::make_unique<Controller>(i);
            if (controller->IsConnected())
                m_controllers.emplace(i, std::move(controller));
        }
    }
}

bool dae::InputManager::ProcessInput()
{
    ScanControllers();
    for (auto& [index, controller] : m_controllers)
        controller->Update();

    std::for_each(m_controllerCommands.begin(), m_controllerCommands.end(),
        [&](const auto& pair)
        {
            const auto& [key, command] = pair;
            const auto& [controllerKey, keyState] = key;
            const auto& [controllerIndex, button] = controllerKey;

            auto it = m_controllers.find(controllerIndex);
            if (it == m_controllers.end())
                return;

            auto& controller = it->second;
            switch (keyState)
            {
            case KeyState::Down:    if (controller->IsDownThisFrame(button)) command->Execute(); break;
            case KeyState::Up:      if (controller->IsUpThisFrame(button))   command->Execute(); break;
            case KeyState::Pressed: if (controller->IsPressed(button))       command->Execute(); break;
            }
        });

    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_EVENT_QUIT)
            return false;

        if (e.type == SDL_EVENT_KEY_DOWN || e.type == SDL_EVENT_KEY_UP)
        {
            SDL_Keycode key = e.key.key;
            KeyState state = (e.type == SDL_EVENT_KEY_DOWN) ? KeyState::Down : KeyState::Up;

            auto it = m_keyboardCommands.find({ key, state });
            if (it != m_keyboardCommands.end())
                it->second->Execute();
        }

        ImGui_ImplSDL3_ProcessEvent(&e);
    }

    const bool* keyboardState = SDL_GetKeyboardState(nullptr);
    for (const auto& [key, command] : m_keyboardCommands)
    {
        const auto& [keyCode, keyState] = key;
        if (keyState == KeyState::Pressed && keyboardState[SDL_GetScancodeFromKey(keyCode, nullptr)])
            command->Execute();
    }

    for (const auto& [key, command] : m_thumbstickCommands)
    {
        const auto& [controllerIndex, thumbstick] = key;

        auto it = m_controllers.find(controllerIndex);
        if (it == m_controllers.end())
            continue;

        auto& controller = it->second;
        glm::vec2 axis = (thumbstick == Thumbstick::Left)
            ? controller->GetLeftThumbstick()
            : controller->GetRightThumbstick();

        command->Execute(axis);
            
    }

    return true;
}
