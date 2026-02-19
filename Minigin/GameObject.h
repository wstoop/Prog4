#pragma once
#include <memory>
#include <vector>

namespace dae
{
	class Component;
	class GameObject  final
	{
		std::vector<std::unique_ptr<Component>> m_components{};
	public:
		void Update();
		void LateUpdate();
		void FixedUpdate();
		void Render() const;

		bool m_destroy{ false };
		template<typename T>
		T* GetComponent() const
		{
			for (const auto& component : m_components)
			{
				if (auto casted = dynamic_cast<T*>(component.get()))
				{
					return casted;
				}
			}

			return nullptr;
		}
		template<typename T, typename... Args>
		T* AddComponent(Args&&... args)
		{
			static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

			auto component = std::make_unique<T>(this, std::forward<Args>(args)...);
			T* rawPtr = component.get();

			m_components.push_back(std::move(component));

			return rawPtr;
		}


		template <typename T>
		void RemoveComponent()
		{
			m_components.erase(
				std::remove_if(m_components.begin(), m_components.end(),
					[](const std::unique_ptr<Component>& component)
					{
						return dynamic_cast<T>(*component) != nullptr;
					}),
				m_components.end());
		}

		GameObject();
		~GameObject();
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;
	};
}