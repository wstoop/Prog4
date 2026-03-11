#include "Controller.h"
#ifdef __EMSCRIPTEN__
#include <SDL.h>

class Controller::ControllerImpl
{
    SDL_Gamepad* m_pGamepad;
    uint32_t      m_controllerIndex;
    uint16_t      m_previousButtons{ 0 };
    uint16_t      m_currentButtons{ 0 };
    uint16_t      m_buttonsPressedThisFrame{ 0 };
    uint16_t      m_buttonsReleasedThisFrame{ 0 };

    static constexpr float k_deadZone = 0.2f;

    static bool SDLButtonPressed(SDL_Gamepad* pad, SDL_GamepadButton sdlBtn)
    {
        return SDL_GetGamepadButton(pad, sdlBtn) != 0;
    }

    uint16_t SampleButtons() const
    {
        if (!m_pGamepad) return 0;
        uint16_t state = 0;
        if (SDLButtonPressed(m_pGamepad, SDL_GAMEPAD_BUTTON_DPAD_UP))
            state |= static_cast<uint16_t>(Controller::Button::DpadUp);
        if (SDLButtonPressed(m_pGamepad, SDL_GAMEPAD_BUTTON_DPAD_DOWN))
            state |= static_cast<uint16_t>(Controller::Button::DpadDown);
        if (SDLButtonPressed(m_pGamepad, SDL_GAMEPAD_BUTTON_DPAD_LEFT))
            state |= static_cast<uint16_t>(Controller::Button::DpadLeft);
        if (SDLButtonPressed(m_pGamepad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT))
            state |= static_cast<uint16_t>(Controller::Button::DpadRight);
        if (SDLButtonPressed(m_pGamepad, SDL_GAMEPAD_BUTTON_START))
            state |= static_cast<uint16_t>(Controller::Button::Start);
        if (SDLButtonPressed(m_pGamepad, SDL_GAMEPAD_BUTTON_BACK))
            state |= static_cast<uint16_t>(Controller::Button::Back);
        if (SDLButtonPressed(m_pGamepad, SDL_GAMEPAD_BUTTON_LEFT_STICK))
            state |= static_cast<uint16_t>(Controller::Button::LeftThumb);
        if (SDLButtonPressed(m_pGamepad, SDL_GAMEPAD_BUTTON_RIGHT_STICK))
            state |= static_cast<uint16_t>(Controller::Button::RightThumb);
        if (SDLButtonPressed(m_pGamepad, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER))
            state |= static_cast<uint16_t>(Controller::Button::LeftShoulder);
        if (SDLButtonPressed(m_pGamepad, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER))
            state |= static_cast<uint16_t>(Controller::Button::RightShoulder);
        if (SDLButtonPressed(m_pGamepad, SDL_GAMEPAD_BUTTON_SOUTH))
            state |= static_cast<uint16_t>(Controller::Button::ButtonA);
        if (SDLButtonPressed(m_pGamepad, SDL_GAMEPAD_BUTTON_EAST))
            state |= static_cast<uint16_t>(Controller::Button::ButtonB);
        if (SDLButtonPressed(m_pGamepad, SDL_GAMEPAD_BUTTON_WEST))
            state |= static_cast<uint16_t>(Controller::Button::ButtonX);
        if (SDLButtonPressed(m_pGamepad, SDL_GAMEPAD_BUTTON_NORTH))
            state |= static_cast<uint16_t>(Controller::Button::ButtonY);
        return state;
    }

    static float ApplyDeadzone(float v)
    {
        return (v > k_deadZone || v < -k_deadZone) ? v : 0.f;
    }

public:
    explicit ControllerImpl(uint32_t index)
        : m_controllerIndex(index)
    {
        int count = 0;
        SDL_JoystickID* joysticks = SDL_GetGamepads(&count);
        if (joysticks && static_cast<int>(index) < count)
            m_pGamepad = SDL_OpenGamepad(joysticks[index]);
        else
            m_pGamepad = nullptr;
        SDL_free(joysticks);
    }

    ~ControllerImpl()
    {
        if (m_pGamepad)
            SDL_CloseGamepad(m_pGamepad);
    }

    void Update()
    {
        m_previousButtons = m_currentButtons;
        m_currentButtons = SampleButtons();

        uint16_t changes = m_currentButtons ^ m_previousButtons;
        m_buttonsPressedThisFrame = changes & m_currentButtons;
        m_buttonsReleasedThisFrame = changes & (~m_currentButtons);
    }

    bool IsConnected() const
    {
        return m_pGamepad != nullptr && SDL_GamepadConnected(m_pGamepad);
    }

    bool IsDownThisFrame(Controller::Button button) const
    {
        return (m_buttonsPressedThisFrame & static_cast<uint16_t>(button)) != 0;
    }

    bool IsUpThisFrame(Controller::Button button) const
    {
        return (m_buttonsReleasedThisFrame & static_cast<uint16_t>(button)) != 0;
    }

    bool IsPressed(Controller::Button button) const
    {
        return (m_currentButtons & static_cast<uint16_t>(button)) != 0;
    }

    glm::vec2 GetLeftThumbstick() const
    {
        if (!m_pGamepad) return { 0.f, 0.f };
        float x = SDL_GetGamepadAxis(m_pGamepad, SDL_GAMEPAD_AXIS_LEFTX) / 32767.f;
        float y = SDL_GetGamepadAxis(m_pGamepad, SDL_GAMEPAD_AXIS_LEFTY) / 32767.f;
        return { ApplyDeadzone(x), ApplyDeadzone(-y) };
    }

    glm::vec2 GetRightThumbstick() const
    {
        if (!m_pGamepad) return { 0.f, 0.f };
        float x = SDL_GetGamepadAxis(m_pGamepad, SDL_GAMEPAD_AXIS_RIGHTX) / 32767.f;
        float y = SDL_GetGamepadAxis(m_pGamepad, SDL_GAMEPAD_AXIS_RIGHTY) / 32767.f;
        return { ApplyDeadzone(x), ApplyDeadzone(-y) };
    }

    glm::vec2 GetDPad() const
    {
        glm::vec2 dir{ 0.f, 0.f };
        if (IsPressed(Controller::Button::DpadRight)) dir.x += 1.f;
        if (IsPressed(Controller::Button::DpadLeft))  dir.x -= 1.f;
        if (IsPressed(Controller::Button::DpadUp))    dir.y += 1.f;
        if (IsPressed(Controller::Button::DpadDown))  dir.y -= 1.f;
        return dir;
    }
};

#else
#include <windows.h>
#include <XInput.h>
#include <glm/glm.hpp>

class Controller::ControllerImpl
{
    XINPUT_STATE m_previousState{};
    XINPUT_STATE m_currentState{};
    uint32_t     m_controllerIndex;
    uint16_t     m_buttonsPressedThisFrame{ 0 };
    uint16_t     m_buttonsReleasedThisFrame{ 0 };

    static constexpr float k_deadZone = 32767.f * 0.2f;

    static float ApplyDeadzone(float v)
    {
        return (v > k_deadZone || v < -k_deadZone) ? v / 32767.f : 0.f;
    }

public:
    explicit ControllerImpl(uint32_t index) : m_controllerIndex(index) {}

    void Update()
    {
        CopyMemory(&m_previousState, &m_currentState, sizeof(XINPUT_STATE));
        ZeroMemory(&m_currentState, sizeof(XINPUT_STATE));
        if (XInputGetState(m_controllerIndex, &m_currentState) != ERROR_SUCCESS)
            return;

        uint16_t changes = m_currentState.Gamepad.wButtons ^ m_previousState.Gamepad.wButtons;
        m_buttonsPressedThisFrame = changes & m_currentState.Gamepad.wButtons;
        m_buttonsReleasedThisFrame = changes & (~m_currentState.Gamepad.wButtons);
    }

    bool IsConnected() const
    {
        XINPUT_STATE state{};
        return XInputGetState(m_controllerIndex, &state) == ERROR_SUCCESS;
    }

    bool IsDownThisFrame(Controller::Button button) const
    {
        return (m_buttonsPressedThisFrame & static_cast<uint16_t>(button)) != 0;
    }

    bool IsUpThisFrame(Controller::Button button) const
    {
        return (m_buttonsReleasedThisFrame & static_cast<uint16_t>(button)) != 0;
    }

    bool IsPressed(Controller::Button button) const
    {
        return (m_currentState.Gamepad.wButtons & static_cast<uint16_t>(button)) != 0;
    }

    glm::vec2 GetLeftThumbstick() const
    {
        return { ApplyDeadzone(m_currentState.Gamepad.sThumbLX),
                 ApplyDeadzone(m_currentState.Gamepad.sThumbLY) };
    }

    glm::vec2 GetRightThumbstick() const
    {
        return { ApplyDeadzone(m_currentState.Gamepad.sThumbRX),
                 ApplyDeadzone(m_currentState.Gamepad.sThumbRY) };
    }

    glm::vec2 GetDPad() const
    {
        glm::vec2 dir{ 0.f, 0.f };
        if (IsPressed(Controller::Button::DpadRight))
            dir.x += 1.f;
        if (IsPressed(Controller::Button::DpadLeft))
            dir.x -= 1.f;
        if (IsPressed(Controller::Button::DpadUp))
            dir.y += 1.f;
        if (IsPressed(Controller::Button::DpadDown))
            dir.y -= 1.f;
        return dir;
    }
};

#endif

Controller::Controller(uint32_t controllerIndex)
	: m_impl(std::make_unique<ControllerImpl>(controllerIndex))
{
}

Controller::~Controller() = default;
void Controller::Update()
{
	m_impl->Update();
}

bool Controller::IsDownThisFrame(Button button) const
{
	return m_impl->IsDownThisFrame(button);
}

bool Controller::IsUpThisFrame(Button button) const
{
	return m_impl->IsUpThisFrame(button);
}

bool Controller::IsPressed(Button button) const
{
	return m_impl->IsPressed(button);
}

glm::vec2 Controller::GetLeftThumbstick() const
{
	return m_impl->GetLeftThumbstick();
}

glm::vec2 Controller::GetRightThumbstick() const
{
	return m_impl->GetRightThumbstick();
}

bool Controller::IsConnected() const
{
    return m_impl->IsConnected();
}

glm::vec2 Controller::GetDPad() const
{
    return m_impl->GetDPad();
}