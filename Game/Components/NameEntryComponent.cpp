#include "NameEntryComponent.h"
#include "Components/TextComponent.h"
#include <algorithm>

namespace
{
    constexpr SDL_Color kSelectedColor{ 255, 230, 0, 255 };
    constexpr SDL_Color kIdleColor{ 255, 255, 255, 255 };
}

dae::NameEntryComponent::NameEntryComponent(GameObject* owner, std::vector<TextComponent*> letterTexts)
    : Component(owner)
    , m_letterTexts(std::move(letterTexts))
{
    Refresh();
}

void dae::NameEntryComponent::MoveSlot(int dir)
{
    m_slot = std::clamp(m_slot + dir, 0, static_cast<int>(m_letterTexts.size()) - 1);
    Refresh();
}

void dae::NameEntryComponent::CycleLetter(int dir)
{
    int letter = m_letters[m_slot] - 'A';
    letter = (letter + dir + 26) % 26;
    m_letters[m_slot] = static_cast<char>('A' + letter);
    Refresh();
}

std::string dae::NameEntryComponent::GetName() const
{
    return { m_letters[0], m_letters[1], m_letters[2] };
}

void dae::NameEntryComponent::Refresh()
{
    for (size_t i = 0; i < m_letterTexts.size(); ++i)
    {
        m_letterTexts[i]->SetText(std::string(1, m_letters[i]));
        m_letterTexts[i]->SetColor(static_cast<int>(i) == m_slot ? kSelectedColor : kIdleColor);
    }
}
