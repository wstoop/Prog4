#pragma once
#include "../GameObject.h"
#include <glm/glm.hpp>

class Command
{
public:
    virtual ~Command() = default;
    virtual void Execute() = 0;
};

class AxisCommand
{
public:
    virtual ~AxisCommand() = default;
    virtual void Execute(glm::vec2 axis) = 0;
};

template<typename Base>
class GameObjectCommandBase : public Base
{
    dae::GameObject* m_gameObject;
protected:
    dae::GameObject* GetGameObject() const { return m_gameObject; }
public:
    GameObjectCommandBase(dae::GameObject* gameObject) : m_gameObject(gameObject) {}
    virtual ~GameObjectCommandBase() = default;
};

using GameObjectCommand = GameObjectCommandBase<Command>;
using GameObjectAxisCommand = GameObjectCommandBase<AxisCommand>;