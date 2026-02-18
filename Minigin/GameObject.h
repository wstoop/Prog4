#pragma once
#include <memory>
#include <vector>

namespace dae
{
	class Component;
	class GameObject  final
	{
		std::vector<std::shared_ptr<Component>> m_components{};
	public:
		void Update();
		void LateUpdate();
		void FixedUpdate();
		void Render() const;

		bool m_destroy{ false };
		template <typename T>
		std::shared_ptr<T> GetComponent() const
		{
			auto it = std::find_if(
				m_components.begin(),
				m_components.end(),
				[](const std::shared_ptr<Component>& component)
				{
					return std::dynamic_pointer_cast<T>(component) != nullptr;
				});

			if (it != m_components.end())
			{
				return std::dynamic_pointer_cast<T>(*it);
			}

			return nullptr;
		}
		template <typename T, typename... Args>
		std::shared_ptr<T> AddComponent(Args&&... args)
		{
			static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
			auto component = std::make_shared<T>(std::forward<Args>(args)...);
			component->SetOwner(this);
			m_components.push_back(component);
			return component;
		}

		template <typename T>
		void RemoveComponent()
		{
			m_components.erase(
				std::remove_if(m_components.begin(), m_components.end(),
					[](const std::shared_ptr<Component>& component)
					{
						return std::dynamic_pointer_cast<T>(component) != nullptr;
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