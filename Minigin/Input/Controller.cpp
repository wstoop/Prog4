#include "Controller.h"
#ifdef __EMSCRIPTEN__
#include <SDL.h>

class Controller::ControllerImpl
{
    SDL_GameController* m_pGamepad;
    uint32_t m_controllerIndex;
    uint16_t m_previousButtons{ 0 };
    uint16_t m_currentButtons{ 0 };
    uint16_t m_buttonsPressedThisFrame{ 0 };
    uint16_t m_buttonsReleasedThisFrame{ 0 };

    static constexpr float k_deadZone = 0.2f;

    static bool SDLButtonPressed(SDL_GameController* pad, SDL_GameControllerButton btn)
    {
        return SDL_GameControllerGetButton(pad, btn) != 0;
    }

    uint16_t SampleButtons() const
    {
        if (!m_pGamepad) return 0;
        uint16_t state = 0;
        if (SDLButtonPressed(m_pGamepad, SDL_CONTROLLER_BUTTON_DPAD_UP))
            state |= static_cast<uint16_t>(Controller::Button::DpadUp);
        if (SDLButtonPressed(m_pGamepad, SDL_CONTROLLER_BUTTON_DPAD_DOWN))
            state |= static_cast<uint16_t>(Controller::Button::DpadDown);
        if (SDLButtonPressed(m_pGamepad, SDL_CONTROLLER_BUTTON_DPAD_LEFT))
            state |= static_cast<uint16_t>(Controller::Button::DpadLeft);
        if (SDLButtonPressed(m_pGamepad, SDL_CONTROLLER_BUTTON_DPAD_RIGHT))
            state |= static_cast<uint16_t>(Controller::Button::DpadRight);
        if (SDLButtonPressed(m_pGamepad, SDL_CONTROLLER_BUTTON_START))
            state |= static_cast<uint16_t>(Controller::Button::Start);
        if (SDLButtonPressed(m_pGamepad, SDL_CONTROLLER_BUTTON_BACK))
            state |= static_cast<uint16_t>(Controller::Button::Back);
        if (SDLButtonPressed(m_pGamepad, SDL_CONTROLLER_BUTTON_LEFTSTICK))
            state |= static_cast<uint16_t>(Controller::Button::LeftThumb);
        if (SDLButtonPressed(m_pGamepad, SDL_CONTROLLER_BUTTON_RIGHTSTICK))
            state |= static_cast<uint16_t>(Controller::Button::RightThumb);
        if (SDLButtonPressed(m_pGamepad, SDL_CONTROLLER_BUTTON_LEFTSHOULDER))
            state |= static_cast<uint16_t>(Controller::Button::LeftShoulder);
        if (SDLButtonPressed(m_pGamepad, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER))
            state |= static_cast<uint16_t>(Controller::Button::RightShoulder);
        if (SDLButtonPressed(m_pGamepad, SDL_CONTROLLER_BUTTON_A))
            state |= static_cast<uint16_t>(Controller::Button::ButtonA);
        if (SDLButtonPressed(m_pGamepad, SDL_CONTROLLER_BUTTON_B))
            state |= static_cast<uint16_t>(Controller::Button::ButtonB);
        if (SDLButtonPressed(m_pGamepad, SDL_CONTROLLER_BUTTON_X))
            state |= static_cast<uint16_t>(Controller::Button::ButtonX);
        if (SDLButtonPressed(m_pGamepad, SDL_CONTROLLER_BUTTON_Y))
            state |= static_cast<uint16_t>(Controller::Button::ButtonY);
        return state;
    }

    static float ApplyDeadzone(float v)
    {
        return (v > k_deadZone || v < -k_deadZone) ? v : 0.f;
    }

public:
    explicit ControllerImpl(uint32_t index)
        : m_controllerIndex(index), m_pGamepad(nullptr)
    {
        if (SDL_IsGameController(static_cast<int>(index)))
            m_pGamepad = SDL_GameControllerOpen(static_cast<int>(index));
    }

    ~ControllerImpl()
    {
        if (m_pGamepad)
            SDL_GameControllerClose(m_pGamepad);
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
        return m_pGamepad != nullptr && SDL_GameControllerGetAttached(m_pGamepad);
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
        float x = SDL_GameControllerGetAxis(m_pGamepad, SDL_CONTROLLER_AXIS_LEFTX) / 32767.f;
        float y = SDL_GameControllerGetAxis(m_pGamepad, SDL_CONTROLLER_AXIS_LEFTY) / 32767.f;
        return { ApplyDeadzone(x), ApplyDeadzone(-y) };
    }

    glm::vec2 GetRightThumbstick() const
    {
        if (!m_pGamepad) return { 0.f, 0.f };
        float x = SDL_GameControllerGetAxis(m_pGamepad, SDL_CONTROLLER_AXIS_RIGHTX) / 32767.f;
        float y = SDL_GameControllerGetAxis(m_pGamepad, SDL_CONTROLLER_AXIS_RIGHTY) / 32767.f;
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
    : m_impl(std::make_unique<ControllerImpl>(controllerIndex)) {
}

Controller::~Controller() = default;

void Controller::Update()
{ 
    m_impl->Update();
}
bool Controller::IsConnected() const
{
    return m_impl->IsConnected();
}
bool Controller::IsDownThisFrame(Button b) const
{
    return m_impl->IsDownThisFrame(b);
}
bool Controller::IsUpThisFrame(Button b) const
{
    return m_impl->IsUpThisFrame(b);
}
bool Controller::IsPressed(Button b) const
{
    return m_impl->IsPressed(b);
}
glm::vec2 Controller::GetLeftThumbstick() const
{
    return m_impl->GetLeftThumbstick();
}
glm::vec2 Controller::GetRightThumbstick() const
{
    return m_impl->GetRightThumbstick();
}
glm::vec2 Controller::GetDPad() const
{
    return m_impl->GetDPad();
}