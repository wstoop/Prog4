#include <algorithm>
#include "Scene.h"
//#include "BulletPool.h"
//#include "ExplosionPool.h"
#include "EventManager.h"
#include <assert.h>
using namespace dae;

void Scene::Add(std::unique_ptr<GameObject> object)
{
	assert(object != nullptr && "Cannot add a null GameObject to the scene.");
	m_objects.emplace_back(std::move(object));
}

void Scene::Remove(GameObject& object)
{
	object.m_destroy = true;
}

void Scene::RemoveAll()
{
	std::for_each(m_objects.begin(), m_objects.end(), [](const auto& obj) {
		obj->m_destroy = true;
		});
}

void Scene::Update()
{
	// Index-based: a component's Update() may synchronously trigger an event
	// that adds a new GameObject to this scene (e.g. the captured-fighter
	// spawner), which can reallocate m_objects and invalidate cached
	// begin()/end() iterators.
	for (size_t i = 0; i < m_objects.size(); ++i)
		m_objects[i]->Update();
}

void Scene::LateUpdate()
{
	for (size_t i = 0; i < m_objects.size(); ++i)
		m_objects[i]->LateUpdate();
 	EventManager::GetInstance().Update();
	PurgeDestroyedObjects();
}

void Scene::FixedUpdate()
{
	for (size_t i = 0; i < m_objects.size(); ++i)
		m_objects[i]->FixedUpdate();
}

void Scene::Render() const
{
	for (size_t i = 0; i < m_objects.size(); ++i)
		m_objects[i]->Render();
}

void Scene::PurgeDestroyedObjects()
{
	m_objects.erase(
		std::remove_if(
			m_objects.begin(),
			m_objects.end(),
			[](const auto& ptr) { return ptr->m_destroy; }
		),
		m_objects.end()
	);
}

void Scene::Reserve(size_t extraCapacity)
{
	m_objects.reserve(m_objects.size() + extraCapacity);
}

