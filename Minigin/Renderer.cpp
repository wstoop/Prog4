#include <stdexcept>
#include <cstring>
#include <iostream>
#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>
#include "Renderer.h"
#include "SceneManager.h"
#include "Texture2D.h"

void dae::Renderer::Init(SDL_Window* window)
{
	m_window = window;

	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

#if defined(__EMSCRIPTEN__)
	m_renderer = SDL_CreateRenderer(window, nullptr);
#else
	m_renderer = SDL_CreateRenderer(window, nullptr);
#endif

	if (m_renderer == nullptr)
	{
		std::cout << "Failed to create the renderer: " << SDL_GetError() << "\n";
		throw std::runtime_error(std::string("SDL_CreateRenderer Error: ") + SDL_GetError());
	}
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui_ImplSDL3_InitForSDLRenderer(window, m_renderer);
	ImGui_ImplSDLRenderer3_Init(m_renderer);
}

void dae::Renderer::Render() const
{

	const auto& color = GetBackgroundColor();
	SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(m_renderer);

	SceneManager::GetInstance().Render();
	if(ImGui::GetDrawData())
		ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), m_renderer);
	SDL_RenderPresent(m_renderer);
	
}

void dae::Renderer::Destroy()
{
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	if (m_renderer != nullptr)
	{
		SDL_DestroyRenderer(m_renderer);
		m_renderer = nullptr;
	}
}

void dae::Renderer::RenderTexture(const Texture2D& texture, const SDL_FRect& src, const SDL_FRect& dst, const float rotation, const float centerX, const float centerY) const
{

	SDL_FPoint center{ centerX, centerY };
	SDL_RenderTextureRotated(GetSDLRenderer(), texture.GetSDLTexture(), &src, &dst, static_cast<double>(rotation), &center, SDL_FLIP_NONE);
}

void dae::Renderer::RenderTexture(const Texture2D& texture, const SDL_FRect& dst, const float rotation, const float centerX, const float centerY) const
{

	SDL_FPoint center{ centerX, centerY };
	SDL_RenderTextureRotated(GetSDLRenderer(), texture.GetSDLTexture(), nullptr, &dst, static_cast<double>(rotation), &center, SDL_FLIP_NONE);
}

SDL_Renderer* dae::Renderer::GetSDLRenderer() const { return m_renderer; }
