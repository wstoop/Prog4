#pragma once
#include <glm/glm.hpp>
#include <memory>

class Controller
{
    class ControllerImpl;
    std::unique_ptr<ControllerImpl> m_impl;
public:
    enum class Button : uint16_t
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

    Controller(uint32_t controllerIndex);
    ~Controller();

    void Update();
	bool IsConnected() const;
    bool IsDownThisFrame(Button button) const;
    bool IsUpThisFrame(Button button) const;
    bool IsPressed(Button button) const;

    glm::vec2 GetDPad() const;
    glm::vec2 GetLeftThumbstick() const;
    glm::vec2 GetRightThumbstick() const;
};