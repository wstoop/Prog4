#pragma once
#include <memory>
#include <string>
#include <vector>
#include "GameObject.h"

namespace dae
{
	class Scene final
	{
	public:
		void Add(std::unique_ptr<GameObject> object);
		void Remove(GameObject& object);
		void RemoveAll();

		void Update();
		void LateUpdate();
		void FixedUpdate();

		void Render() const;

		std::string GetName() const { return m_name; }

		~Scene() = default;
		Scene(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) = delete;

	private:
		friend class SceneManager;
		explicit Scene(const std::string& name) { m_name = name; };

		void PurgeDestroyedObjects();
		std::vector < std::unique_ptr<GameObject>> m_objects{};
		std::string m_name{};
	};

}
