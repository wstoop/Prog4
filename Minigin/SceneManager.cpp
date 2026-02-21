#include "SceneManager.h"
#include "Scene.h"

void dae::SceneManager::Update()
{
	if (m_activeScene)
		m_activeScene->Update();
}

void dae::SceneManager::FixedUpdate()
{
	if (m_activeScene)
		m_activeScene->FixedUpdate();
}

void dae::SceneManager::LateUpdate()
{
	if (m_activeScene)
		m_activeScene->LateUpdate();
}

void dae::SceneManager::Render()
{
	if (m_activeScene)
		m_activeScene->Render();
}

dae::Scene& dae::SceneManager::CreateScene(const std::string& name)
{
	m_scenes.emplace_back(new Scene(name));
	return *m_scenes.back();
}

void dae::SceneManager::SetActiveScene(const std::string& name)
{
	for (auto& scene : m_scenes)
	{
		if (scene->GetName() == name)
		{
			m_activeScene = scene.get();
			return;
		}
	}
}

dae::Scene* dae::SceneManager::GetActiveScene() const
{
	return m_activeScene;
}