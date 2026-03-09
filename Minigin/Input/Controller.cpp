#include "Controller.h"
#ifdef __EMSCRIPTEN__
#include <SDL.h>

class Controller::ControllerImpl
{
    SDL_GameController* m_pController;
    uint16_t m_buttonsPressedThisFrame;
    uint16_t m_buttonsReleasedThisFrame;
    uint16_t m_previousButtons;
    uint16_t m_currentButtons;
public:
    ControllerImpl(uint32_t index)
        : m_pController(SDL_GameControllerOpen(index)),
        m_buttonsPressedThisFrame(0), m_buttonsReleasedThisFrame(0),
        m_previousButtons(0), m_currentButtons(0) {
    }

    void Update()
    {
        m_previousButtons = m_currentButtons;
        m_currentButtons = 0;

        // Map SDL buttons to your Button bitmasks
        if (SDL_GameControllerGetButton(m_pController, SDL_CONTROLLER_BUTTON_A))
            m_currentButtons |= static_cast<uint16_t>(Controller::Button::ButtonA);
        // ... etc

        auto buttonChanges = m_currentButtons ^ m_previousButtons;
        m_buttonsPressedThisFrame = buttonChanges & m_currentButtons;
        m_buttonsReleasedThisFrame = buttonChanges & (~m_currentButtons);
    }
};

#else
#include <windows.h>
#include <XInput.h>

class Controller::ControllerImpl
{
    XINPUT_STATE m_previousState;
    XINPUT_STATE m_currentState;
    uint32_t m_controllerIndex;
    uint16_t m_buttonsPressedThisFrame;
    uint16_t m_buttonsReleasedThisFrame;

	float m_deadZone = 32767.f * 0.2f;
public:
    Controller::ControllerImpl(uint32_t);
    void Update();
    bool IsDownThisFrame(Controller::Button button) const;
    bool IsUpThisFrame(Controller::Button button) const;
    bool IsPressed(Controller::Button button) const;
	glm::vec2 GetLeftThumbstick() const;
	glm::vec2 GetRightThumbstick() const;
    glm::vec2 GetDPad() const;
    bool IsConnected() const;
};
Controller::ControllerImpl::ControllerImpl(uint32_t index)
    : m_controllerIndex(index),
    m_previousState{}, m_currentState{},
    m_buttonsPressedThisFrame(0), m_buttonsReleasedThisFrame(0) {
}

void Controller::ControllerImpl::Update()
{
    CopyMemory(&m_previousState, &m_currentState, sizeof(XINPUT_STATE));
    ZeroMemory(&m_currentState, sizeof(XINPUT_STATE));
    XInputGetState(m_controllerIndex, &m_currentState);
    DWORD result = XInputGetState(m_controllerIndex, &m_currentState);
    if (result != ERROR_SUCCESS)
    {
        return;
    }

    auto buttonChanges = m_currentState.Gamepad.wButtons ^ m_previousState.Gamepad.wButtons;
    m_buttonsPressedThisFrame = buttonChanges & m_currentState.Gamepad.wButtons;
    m_buttonsReleasedThisFrame = buttonChanges & (~m_currentState.Gamepad.wButtons);

}

glm::vec2 Controller::ControllerImpl::GetLeftThumbstick() const
{
    float x = m_currentState.Gamepad.sThumbLX;
    float y = m_currentState.Gamepad.sThumbLY;
    if (abs(x) < m_deadZone) x = 0;
    if (abs(y) < m_deadZone) y = 0;

    return { x / 32767.f, y / 32767.f };
}

glm::vec2 Controller::ControllerImpl::GetRightThumbstick() const
{
    float x = m_currentState.Gamepad.sThumbRX;
    float y = m_currentState.Gamepad.sThumbRY;
    if (abs(x) < m_deadZone) x = 0;
    if (abs(y) < m_deadZone) y = 0;

    return { x / 32767.f, y / 32767.f };
}

bool Controller::ControllerImpl::IsConnected() const
{
    XINPUT_STATE state{};
    return XInputGetState(m_controllerIndex, &state) == ERROR_SUCCESS;
}

#endif

bool Controller::ControllerImpl::IsDownThisFrame(Controller::Button button) const
{
    return m_buttonsPressedThisFrame & static_cast<uint16_t>(button);
}
bool Controller::ControllerImpl::IsUpThisFrame(Controller::Button button) const
{
    return m_buttonsReleasedThisFrame & static_cast<uint16_t>(button);
}
bool Controller::ControllerImpl::IsPressed(Controller::Button button) const
{
    return m_currentState.Gamepad.wButtons & static_cast<uint16_t>(button);
}

glm::vec2 Controller::ControllerImpl::GetDPad() const
{
    glm::vec2 dir{ 0.f, 0.f };
    if (IsPressed(Button::DpadRight)) dir.x += 1.f;
    if (IsPressed(Button::DpadLeft))  dir.x -= 1.f;
    if (IsPressed(Button::DpadUp))    dir.y += 1.f;
    if (IsPressed(Button::DpadDown))  dir.y -= 1.f;
    return dir;
}

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