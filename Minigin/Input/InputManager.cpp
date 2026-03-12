#include <SDL3/SDL.h>
#include <backends/imgui_impl_sdl3.h>
#include "InputManager.h"
#include "Controller.h"
#include <map>
#include <tuple>

namespace dae
{
    // ---- Internal key types ----

    struct ControllerBindKey
    {
        uint32_t         controllerIndex;
        ControllerButton button;
        KeyState         state;

        bool operator<(const ControllerBindKey& o) const
        {
            return std::tie(controllerIndex, button, state)
                < std::tie(o.controllerIndex, o.button, o.state);
        }
    };

    struct ThumbstickBindKey
    {
        uint32_t   controllerIndex;
        Thumbstick thumbstick;

        bool operator<(const ThumbstickBindKey& o) const
        {
            return std::tie(controllerIndex, thumbstick)
                < std::tie(o.controllerIndex, o.thumbstick);
        }
    };

    struct KeyboardBindKey
    {
        int      scancode;
        KeyState state;

        bool operator<(const KeyboardBindKey& o) const
        {
            return std::tie(scancode, state) < std::tie(o.scancode, o.state);
        }
    };

    // ---- Pimpl ----

    struct InputManager::Impl
    {
        uint32_t m_nextControllerIndex{ 0 };
        std::map<uint32_t, std::unique_ptr<Controller>> m_controllers;

        std::map<ControllerBindKey, std::unique_ptr<Command>>     m_controllerCommands;
        std::map<ThumbstickBindKey, std::unique_ptr<AxisCommand>> m_thumbstickCommands;
        std::map<KeyboardBindKey, std::unique_ptr<Command>>     m_keyboardCommands;
        std::map<KeyboardAxis, std::unique_ptr<AxisCommand>> m_keyboardAxisCommands;

        void ScanControllers()
        {
            for (uint32_t i = 0; i < 4; ++i)
            {
                if (m_controllers.find(i) == m_controllers.end())
                {
                    auto controller = std::make_unique<Controller>(i);
                    if (controller->IsConnected())
                        m_controllers.emplace(i, std::move(controller));
                }
            }
        }

        bool ProcessInput()
        {
            SDL_Event e;
            while (SDL_PollEvent(&e))
            {
                if (e.type == SDL_EVENT_QUIT)
                    return false;

#ifdef __EMSCRIPTEN__
                if (e.type == SDL_EVENT_GAMEPAD_ADDED || e.type == SDL_EVENT_GAMEPAD_REMOVED)
                    ScanControllers();
#endif

                if (e.type == SDL_EVENT_KEY_DOWN || e.type == SDL_EVENT_KEY_UP)
                {
                    if (e.key.repeat) continue;
                    KeyState state = (e.type == SDL_EVENT_KEY_DOWN) ? KeyState::Down : KeyState::Up;
                    KeyboardBindKey key{ static_cast<int>(e.key.scancode), state };
                    auto it = m_keyboardCommands.find(key);
                    if (it != m_keyboardCommands.end())
                        it->second->Execute();
                }

                ImGui_ImplSDL3_ProcessEvent(&e);
            }

            for (auto& [index, controller] : m_controllers)
                controller->Update();

            // Controller button commands
            for (const auto& [key, command] : m_controllerCommands)
            {
                auto it = m_controllers.find(key.controllerIndex);
                if (it == m_controllers.end()) continue;

                auto& ctrl = *it->second;
                auto  btn = static_cast<Controller::Button>(static_cast<uint16_t>(key.button));

                switch (key.state)
                {
                case KeyState::Down:    if (ctrl.IsDownThisFrame(btn)) command->Execute(); break;
                case KeyState::Up:      if (ctrl.IsUpThisFrame(btn))   command->Execute(); break;
                case KeyState::Pressed: if (ctrl.IsPressed(btn))       command->Execute(); break;
                }
            }

            // Keyboard held (Pressed state)
            const auto* kb = SDL_GetKeyboardState(nullptr);
            for (const auto& [key, command] : m_keyboardCommands)
            {
                if (key.state == KeyState::Pressed && kb[key.scancode])
                    command->Execute();
            }

            // Keyboard axis (WASD / arrows)
            for (const auto& [axis, command] : m_keyboardAxisCommands)
            {
                glm::vec2 dir{ 0.f, 0.f };
                if (kb[axis.scancodeRight]) dir.x += 1.f;
                if (kb[axis.scancodeLeft])  dir.x -= 1.f;
                if (kb[axis.scancodeUp])    dir.y += 1.f;
                if (kb[axis.scancodeDown])  dir.y -= 1.f;
                command->Execute(dir);
            }

            // Thumbsticks and DPad
            for (const auto& [key, command] : m_thumbstickCommands)
            {
                auto it = m_controllers.find(key.controllerIndex);
                if (it == m_controllers.end()) continue;

                auto& ctrl = *it->second;
                glm::vec2 axis{};
                switch (key.thumbstick)
                {
                case Thumbstick::Left:  axis = ctrl.GetLeftThumbstick();  break;
                case Thumbstick::Right: axis = ctrl.GetRightThumbstick(); break;
                case Thumbstick::DPad:  axis = ctrl.GetDPad();            break;
                }
                command->Execute(axis);
            }

            return true;
        }
    };

    // ---- Public interface ----

    InputManager::InputManager() : m_impl(std::make_unique<Impl>())
    {
#ifdef __EMSCRIPTEN__
        m_impl->ScanControllers();
#endif
    }
    InputManager::~InputManager() = default;

    uint32_t InputManager::AddController()
    {
        uint32_t index = m_impl->m_nextControllerIndex++;
        m_impl->m_controllers.emplace(index, std::make_unique<Controller>(index));
        return index;
    }

    void InputManager::BindCommand(uint32_t controllerIndex, ControllerButton button,
        KeyState state, std::unique_ptr<Command> command)
    {
        m_impl->m_controllerCommands[{ controllerIndex, button, state }] = std::move(command);
    }

    void InputManager::UnbindCommand(uint32_t controllerIndex, ControllerButton button, KeyState state)
    {
        m_impl->m_controllerCommands.erase({ controllerIndex, button, state });
    }

    void InputManager::BindCommand(uint32_t controllerIndex, Thumbstick thumbstick,
        std::unique_ptr<AxisCommand> command)
    {
        m_impl->m_thumbstickCommands[{ controllerIndex, thumbstick }] = std::move(command);
    }

    void InputManager::UnbindCommand(uint32_t controllerIndex, Thumbstick thumbstick)
    {
        m_impl->m_thumbstickCommands.erase({ controllerIndex, thumbstick });
    }

    void InputManager::BindCommand(int scancode, KeyState state, std::unique_ptr<Command> command)
    {
        m_impl->m_keyboardCommands[{ scancode, state }] = std::move(command);
    }

    void InputManager::UnbindCommand(int scancode, KeyState state)
    {
        m_impl->m_keyboardCommands.erase({ scancode, state });
    }

    void InputManager::BindCommand(KeyboardAxis axis, std::unique_ptr<AxisCommand> command)
    {
        m_impl->m_keyboardAxisCommands[axis] = std::move(command);
    }

    void InputManager::UnbindCommand(KeyboardAxis axis)
    {
        m_impl->m_keyboardAxisCommands.erase(axis);
    }

    bool InputManager::ProcessInput()
    {
        return m_impl->ProcessInput();
    }
}