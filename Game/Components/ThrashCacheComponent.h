#pragma once
#include "Components/RenderComponent.h"
#include <vector>
#include <functional>
#include <chrono>
#include <algorithm>

namespace dae
{
	class Font;
	class Texture2D;

	struct Transform
	{
		float matrix[16] = {
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1 };
	};

	class GameObject3D
	{
	public:
		Transform transform;
		int ID;
	};

	class GameObject3DAlt
	{
	public:
		Transform* transform;
		int ID;
	};

	class ThrashCacheComponent final : public RenderComponent
	{
	public:
		void Render() override;
		ThrashCacheComponent(GameObject* owner);
		~ThrashCacheComponent() override = default;
		ThrashCacheComponent(const ThrashCacheComponent& other) = delete;
		ThrashCacheComponent(ThrashCacheComponent&& other) = delete;
		ThrashCacheComponent& operator=(const ThrashCacheComponent& other) = delete;
		ThrashCacheComponent& operator=(ThrashCacheComponent&& other) = delete;
	private:
		std::vector<float> m_steps;
		std::vector<float> m_TimingsEx1;
		std::vector<float> m_TimingsEx2;
		std::vector<float> m_TimingsEx2Alt;

		int m_numSamplesEx1{10};
		int m_numSamplesEx2{10};

		void DisplayEx1();
		void DisplayEx2();

		void RunEx1();
		void RunEx2();
		void RunEx2Alt();

		template<typename T>
		std::vector<float> RunCacheThrash(
			size_t N,
			int numSamples,
			const std::function<void(T&, size_t)>& operation,
			std::vector<float>& outSteps)
		{
			std::vector<T> arr(N);
			std::vector<float> timings;
			outSteps.clear();

			for (size_t step = 1; step <= 1024; step *= 2)
			{
				std::vector<float> samples;

				for (int s = 0; s < numSamples; ++s)
				{
					auto start = std::chrono::high_resolution_clock::now();

					for (size_t i = 0; i < N; i += step)
						operation(arr[i], i);

					auto end = std::chrono::high_resolution_clock::now();
					std::chrono::duration<float, std::milli> duration = end - start;
					samples.push_back(duration.count());
				}

				std::sort(samples.begin(), samples.end());
				int remove = static_cast<int>(samples.size() * 0.1f);
				float sum = 0;
				for (int i = remove; i < int(samples.size()) - remove; ++i)
					sum += samples[i];

				float avg = sum / (samples.size() - 2 * remove);
				timings.push_back(avg);
				outSteps.push_back(static_cast<float>(step));
			}

			return timings;
		}
	};
}