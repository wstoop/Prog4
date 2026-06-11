#pragma once
#include "GameObject.h"

struct UISelection
{
    UISelection(dae::GameObject* start) : current(start) {}
    dae::GameObject* current{ nullptr };
};