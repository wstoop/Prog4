#include <algorithm>
#include "Scene.h"
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
	std::for_each(m_objects.begin(), m_objects.end(), [](const auto& obj) {
		obj->Update();
		});
}

void Scene::LateUpdate()
{
	std::for_each(m_objects.begin(), m_objects.end(), [](const auto& obj) {
		obj->LateUpdate();
		});
	PurgeDestroyedObjects();
}

void Scene::FixedUpdate()
{
	std::for_each(m_objects.begin(), m_objects.end(), [](const auto& obj) {
		obj->FixedUpdate();
		});
}

void Scene::Render() const
{
	std::for_each(m_objects.begin(), m_objects.end(), [](const auto& obj) {
		obj->Render();
		});
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

