#include <SDL3/SDL.h>
#include <backends/imgui_impl_sdl3.h>
#include "InputManager.h"
#include "Controller.h"
#include <map>
#include <tuple>
#include <vector>

namespace dae
{

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

    struct InputManager::Impl
    {
        std::map<uint32_t, std::unique_ptr<Controller>> m_controllers;

        std::map<ControllerBindKey, std::unique_ptr<Command>>     m_controllerCommands;
        std::map<ThumbstickBindKey, std::unique_ptr<AxisCommand>> m_thumbstickCommands;
        std::map<KeyboardBindKey, std::unique_ptr<Command>>     m_keyboardCommands;
        std::map<std::pair<KeyboardAxis, KeyState>, std::unique_ptr<AxisCommand>> m_keyboardAxisCommands;

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
                    int scancode = static_cast<int>(e.key.scancode);

                    // 1. Fire traditional single-key actions (e.g., Spacebar to select)
                    KeyboardBindKey key{ scancode, state };
                    auto it = m_keyboardCommands.find(key);
                    if (it != m_keyboardCommands.end())
                        it->second->Execute();

                    // 2. NEW: Look up and fire Axis actions bound to discrete states (Down / Up)
                    for (const auto& [axisPair, command] : m_keyboardAxisCommands)
                    {
                        const KeyboardAxis& axis = axisPair.first;
                        KeyState boundState = axisPair.second;

                        // Only evaluate this axis if its registered state matches the event state
                        if (boundState == state)
                        {
                            glm::vec2 dir{ 0.f, 0.f };
                            bool axisKeyTriggered = false;

                            // Evaluate which direction key matched the event's scancode
                            if (scancode == axis.scancodeRight) { dir.x += 1.f; axisKeyTriggered = true; }
                            if (scancode == axis.scancodeLeft) { dir.x -= 1.f; axisKeyTriggered = true; }
                            if (scancode == axis.scancodeUp) { dir.y += 1.f; axisKeyTriggered = true; }
                            if (scancode == axis.scancodeDown) { dir.y -= 1.f; axisKeyTriggered = true; }

                            // Only fire if this specific axis was the one interacted with
                            if (axisKeyTriggered)
                            {
                                command->Execute(dir);
                            }
                        }
                    }
                }

                ImGui_ImplSDL3_ProcessEvent(&e);
            }

            for (auto& [index, controller] : m_controllers)
                controller->Update();

            // Controller button commands.
            // Collect the commands to fire first, then execute them after the
            // iteration completes - a command's Execute() may itself call
            // Bind/UnbindCommand (e.g. confirming name entry), which would
            // otherwise erase the very map node being iterated and crash.
            std::vector<Command*> controllerCommandsToFire;
            for (const auto& [key, command] : m_controllerCommands)
            {
                auto it = m_controllers.find(key.controllerIndex);
                if (it == m_controllers.end()) continue;

                auto& ctrl = *it->second;
                auto  btn = static_cast<Controller::Button>(static_cast<uint16_t>(key.button));

                bool fire = false;
                switch (key.state)
                {
                case KeyState::Down:    fire = ctrl.IsDownThisFrame(btn); break;
                case KeyState::Up:      fire = ctrl.IsUpThisFrame(btn);   break;
                case KeyState::Pressed: fire = ctrl.IsPressed(btn);       break;
                }
                if (fire) controllerCommandsToFire.push_back(command.get());
            }
            for (auto* command : controllerCommandsToFire)
                command->Execute();

            // Keyboard held (Pressed state)
            const auto* kb = SDL_GetKeyboardState(nullptr);
            std::vector<Command*> keyboardCommandsToFire;
            for (const auto& [key, command] : m_keyboardCommands)
            {
                if (key.state == KeyState::Pressed && kb[key.scancode])
                    keyboardCommandsToFire.push_back(command.get());
            }
            for (auto* command : keyboardCommandsToFire)
                command->Execute();

            // 3. UPDATED: Process continuous Keyboard Axis states (Pressed state only)
            std::vector<std::pair<AxisCommand*, glm::vec2>> keyboardAxisCommandsToFire;
            for (const auto& [axisPair, command] : m_keyboardAxisCommands)
            {
                const KeyboardAxis& axis = axisPair.first;
                KeyState boundState = axisPair.second;

                // Continuous axis monitoring should ONLY process commands bound to KeyState::Pressed
                if (boundState == KeyState::Pressed)
                {
                    glm::vec2 dir{ 0.f, 0.f };
                    if (kb[axis.scancodeRight]) dir.x += 1.f;
                    if (kb[axis.scancodeLeft])  dir.x -= 1.f;
                    if (kb[axis.scancodeUp])    dir.y += 1.f;
                    if (kb[axis.scancodeDown])  dir.y -= 1.f;

                    keyboardAxisCommandsToFire.emplace_back(command.get(), dir);
                }
            }
            for (auto& [command, dir] : keyboardAxisCommandsToFire)
                command->Execute(dir);

            // Thumbsticks and DPad
            std::vector<std::pair<AxisCommand*, glm::vec2>> thumbstickCommandsToFire;
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
                thumbstickCommandsToFire.emplace_back(command.get(), axis);
            }
            for (auto& [command, axis] : thumbstickCommandsToFire)
                command->Execute(axis);

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

    uint32_t InputManager::AddController(const std::set<uint32_t>& exclude)
    {
        // (Re)scan all slots so already-connected controllers are picked up even
        // if a previous scene never registered them, and reuse any controller
        // that's already registered and still connected.
        m_impl->ScanControllers();

        for (auto& [index, controller] : m_impl->m_controllers)
        {
            if (controller->IsConnected() && exclude.find(index) == exclude.end())
                return index;
        }

        return UINT32_MAX;
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

    void InputManager::BindCommand(KeyboardAxis axis, KeyState state, std::unique_ptr<AxisCommand> command)
    {
        m_impl->m_keyboardAxisCommands[{ axis, state }] = std::move(command);
    }

    void InputManager::UnbindCommand(KeyboardAxis axis, KeyState state)
    {
        m_impl->m_keyboardAxisCommands.erase({ axis, state });
    }

    bool InputManager::ProcessInput()
    {
        return m_impl->ProcessInput();
    }
}