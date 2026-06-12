#pragma once
#include "Components/Component.h"
#include <array>
#include <string>
#include <vector>

namespace dae
{
    class TextComponent;

    class NameEntryComponent final : public Component
    {
    public:
        NameEntryComponent(GameObject* owner, std::vector<TextComponent*> letterTexts);

        void MoveSlot(int dir);
        void CycleLetter(int dir);
        std::string GetName() const;

    private:
        void Refresh();

        std::vector<TextComponent*> m_letterTexts;
        std::array<char, 3> m_letters{ 'A', 'A', 'A' };
        int m_slot{ 0 };
    };
}
