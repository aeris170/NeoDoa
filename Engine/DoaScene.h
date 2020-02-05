// NeoDoa 2019, v0.1 ALPHA
#pragma once

namespace doa::scene {

	class GameObject;
	class DOA_API Scene {
	private:
		char* m_name;
		std::vector<GameObject*>* m_objects;
		std::vector<Light*>* m_lights;
		glm::vec3* m_ambientLight{ new glm::vec3(1, 1, 1) };

		virtual void update();
		virtual void render();

		friend void internal::update();
		friend void internal::render();

	public:
		Scene(const char* name, const std::vector<GameObject*>& objects = std::vector<GameObject*>(), const std::vector<Light*>& lights = std::vector<Light*>());
		virtual ~Scene();

		virtual void Add(GameObject* o);
		virtual void Add(Light* l);
		virtual void Remove(GameObject* o);
		virtual void Remove(Light* l);
		virtual bool Contains(GameObject* o) const;
		virtual bool Contains(Light* l) const;
		virtual size_t Count() const;
		virtual size_t ObjectCount() const;
		virtual size_t LightCount() const;
		virtual bool IsEmpty() const;
		virtual void Clear();

		virtual inline const char* GetName() const { return m_name; }
		virtual inline void SetName(const char* name) {
			if (!name) {
				std::cout << "doa::scene::Scene::SetName cannot accept NULL as name!\n";
				throw - 1;
			}
			if (name[0] == 0) {
				std::cout << "doa::scene::Scene::SetName cannot accept 0-length char* as name!\n";
				throw - 1;
			}
			delete[] m_name;
			int length = strlen(name);
			m_name = new char[length + 1];
			for (int i{ 0 }; i < length + 1; ++i) {
				m_name[i] = name[i];
			}
		}

		virtual inline glm::vec3* GetAmbientLight() const { return m_ambientLight; }
		virtual inline void SetAmbientLight(const glm::vec3& ambientLight) { *m_ambientLight = ambientLight; }
		virtual inline void SetAmbientLight(float r, float g, float b) {
			m_ambientLight->r = r;
			m_ambientLight->g = g;
			m_ambientLight->b = b;
		}
	};

	extern DOA_API std::map<const char*, Scene*, internal::char_cmp> SCENES;
	extern DOA_API Scene* ACTIVE_SCENE;
}

namespace internal::scene {

	void purge();
}