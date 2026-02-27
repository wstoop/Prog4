#include "ThrashCacheComponent.h"
#include "imgui_plot.h"
#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>
#include <chrono>
#include <algorithm>

dae::ThrashCacheComponent::ThrashCacheComponent(GameObject* owner):
	RenderComponent(owner)
{
}

void dae::ThrashCacheComponent::Render()
{

    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    DisplayEx1();
    DisplayEx2();

    ImGui::Render();
}

void dae::ThrashCacheComponent::DisplayEx1()
{
    ImGui::Begin("Exercise 1");

    // Row: number input + buttons + label
    ImGui::PushItemWidth(80); // optional, to limit input box width
    ImGui::InputInt("##numSamples", &m_numSamplesEx1);
    ImGui::PopItemWidth();

    // Make sure number stays positive
    if (m_numSamplesEx1 < 0) m_numSamplesEx1 = 0;

    // Label next to buttons
    ImGui::SameLine();
    ImGui::Text("# of samples");

    // Spacer
    ImGui::Spacing();
    ImGui::Spacing();

    // Button underneath
    if (ImGui::Button("Thrash the cache"))
    {
        RunEx1();
    }

    if (!m_TimingsEx1.empty() && !m_steps.empty()) {
        ImGui::PlotConfig conf;
        conf.values.ys = m_TimingsEx1.data();
        conf.values.count = static_cast<int>(m_TimingsEx1.size());
        conf.values.xs = m_steps.data();
        conf.scale.min = *std::min_element(m_TimingsEx1.begin(), m_TimingsEx1.end()) * 0.95f;
        conf.scale.max = *std::max_element(m_TimingsEx1.begin(), m_TimingsEx1.end()) * 1.05f;
        conf.scale.type = ImGui::PlotConfig::Scale::Log10;
        conf.frame_size = ImVec2(300, 200);
        conf.tooltip.show = true;
        conf.tooltip.format = "Step %g ms: %8.4g";

        ImGui::Plot("Ex1", conf);
    }

    ImGui::End();
}

void dae::ThrashCacheComponent::DisplayEx2()
{
    ImGui::Begin("Exercise 2");

    // Row: number input + buttons + label
    ImGui::PushItemWidth(80); // optional, to limit input box width
    ImGui::InputInt("##numSamples", &m_numSamplesEx2);
    ImGui::PopItemWidth();

    // Make sure number stays positive
    if (m_numSamplesEx2 < 0) m_numSamplesEx2 = 0;

    // Label next to buttons
    ImGui::SameLine();
    ImGui::Text("# of samples");

    // Spacer
    ImGui::Spacing();
    ImGui::Spacing();

    // Button underneath
    if (ImGui::Button("Thrash the cache with GameObject3D"))
    {
        RunEx2();
    }
    if (!m_TimingsEx2.empty() && !m_steps.empty()) {
        ImGui::PlotConfig conf;
        conf.values.ys = m_TimingsEx2.data();
        conf.values.count = static_cast<int>(m_TimingsEx2.size());
        conf.values.xs = m_steps.data();
        conf.scale.min = *std::min_element(m_TimingsEx2.begin(), m_TimingsEx2.end()) * 0.95f;
        conf.scale.max = *std::max_element(m_TimingsEx2.begin(), m_TimingsEx2.end()) * 1.05f;
        conf.scale.type = ImGui::PlotConfig::Scale::Log10;
        conf.frame_size = ImVec2(300, 200);
        conf.tooltip.show = true;
        conf.tooltip.format = "Step %g ms: %8.4g";

        ImGui::Plot("Ex2", conf);
    }
    if (ImGui::Button("Thrash the cache with GameObject3DAlt"))
    {
        RunEx2Alt();
    }
    if (!m_TimingsEx2Alt.empty() && !m_steps.empty()) {
        ImGui::PlotConfig conf;
        conf.values.ys = m_TimingsEx2Alt.data();
        conf.values.count = static_cast<int>(m_TimingsEx2Alt.size());
        conf.values.xs = m_steps.data();
        conf.scale.min = *std::min_element(m_TimingsEx2Alt.begin(), m_TimingsEx2Alt.end()) * 0.95f;
        conf.scale.max = *std::max_element(m_TimingsEx2Alt.begin(), m_TimingsEx2Alt.end()) * 1.05f;
        conf.scale.type = ImGui::PlotConfig::Scale::Log10;
        conf.frame_size = ImVec2(300, 200);
        conf.tooltip.show = true;
        conf.tooltip.format = "Step %g ms: %8.4g";

        ImGui::Plot("Ex2Alt", conf);
    }
    ImGui::End();
}

void dae::ThrashCacheComponent::RunEx1()
{
    const size_t N = 1 << 26;
    m_TimingsEx1 = RunCacheThrash<int>(
        N,
        m_numSamplesEx1,
        [](int& val, size_t) { val *= 2; },
        m_steps
    );
}

void dae::ThrashCacheComponent::RunEx2()
{
    const size_t N = 1 << 22;
    m_TimingsEx2 = RunCacheThrash<GameObject3D>(
        N,
        m_numSamplesEx2,
        [](GameObject3D& obj, size_t) { obj.ID += 1; },
        m_steps
    );
}

void dae::ThrashCacheComponent::RunEx2Alt()
{
    const size_t N = 1 << 22;
    std::vector<GameObject3DAlt> arr(N);
    for (auto& obj : arr)
        obj.transform = new Transform();

    m_TimingsEx2Alt = RunCacheThrash<GameObject3DAlt>(
        N,
        m_numSamplesEx2,
        [](GameObject3DAlt& obj, size_t) { obj.ID += 1; },
        m_steps
    );

    for (auto& obj : arr)
        delete obj.transform;
}