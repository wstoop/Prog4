#pragma once
#include "Singleton.h"

namespace dae
{
    class GameInfo final : public Singleton<GameInfo>
    {
        friend class Singleton<GameInfo>;
        GameInfo() = default;
		int m_screenWidth{};
		int m_gameScreenWidth{};
        int m_screenHeight{};

    public:
        int GetScreenWidth() const { return m_screenWidth; }
        int GetScreenHeight() const { return m_screenHeight; }
		int GetGameWidth() const { return m_gameScreenWidth; }

		void SetScreenDimensions(int width, int height) { m_screenWidth = width; m_screenHeight = height; }
		void SetGameScreenWidth(int width) { m_gameScreenWidth = width; }
    };
}