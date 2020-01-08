// NeoDoa 2019, v0.1 ALPHA
#include "doa.h"

namespace doa::scene {
	using namespace internal::scene;

	std::map<std::string, Scene*> SCENES;
	Scene* ACTIVE_SCENE{ NULL };

	Scene::Scene(const std::string& name, const std::vector<GameObject*>& objects, const std::vector<Light*>& lights) : m_name{ new std::string(name) }, m_objects{ new std::vector(objects) }, m_lights{ new std::vector(lights) } {
		if (SCENES[name]) {
			delete SCENES[name];
		}
		SCENES[name] = this;
	}

	Scene::~Scene() { delete m_name; delete m_objects; delete m_lights; delete m_ambientLight; }

	void Scene::Add(GameObject* o) {
		m_objects->push_back(o);
	}

	void Scene::Add(Light* l) {
		m_lights->push_back(l);
	}

	void Scene::Remove(GameObject* o) {
		for (auto itr{ m_objects->begin() }; itr != m_objects->end(); ++itr) {
			if (*itr == o) {
				m_objects->erase(itr);
				break;
			}
		}
	}

	void Scene::Remove(Light* l) {
		for (auto itr{ m_lights->begin() }; itr != m_lights->end(); ++itr) {
			if (*itr == l) {
				m_lights->erase(itr);
				break;
			}
		}
	}

	bool Scene::Contains(GameObject* o) const {
		for (auto itr{ m_objects->begin() }; itr != m_objects->end(); ++itr) {
			if (*itr == o) {
				return true;
			}
		}
		return false;
	}

	bool Scene::Contains(Light* l) const {
		for (auto itr{ m_lights->begin() }; itr != m_lights->end(); ++itr) {
			if (*itr == l) {
				return true;
			}
		}
		return false;
	}

	size_t Scene::Count() const {
		return m_objects->size() + m_lights->size();
	}

	size_t Scene::ObjectCount() const {
		return m_objects->size();
	}

	size_t Scene::LightCount() const {
		return m_lights->size();
	}

	bool Scene::IsEmpty() const {
		return m_objects->empty() && m_lights->empty();
	}

	void Scene::Clear() {
		m_objects->clear();
		m_lights->clear();
	}

	void Scene::update() {
		std::for_each(m_objects->begin(), m_objects->end(), [&](const GameObject *o) { o->update(*this, m_objects, m_lights); });
	}

	void Scene::render() {
		std::for_each(m_objects->begin(), m_objects->end(), [&](const GameObject *o) { o->render(*this, m_objects, m_lights); });
	}
}

namespace internal::scene {

	void purge() {
		for (auto itr{ doa::scene::SCENES.begin() }; itr != doa::scene::SCENES.end(); ++itr)
		{
			delete (itr->second);
		}
		doa::scene::SCENES.clear();
	}
}