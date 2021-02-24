// NeoDoa 2019, v0.1 ALPHA
#include "doa.h"

namespace doa::physics {
	using namespace internal::physics;

	void SetGravity(const glm::vec2& gravity) {
		m_physics->SetGravity(b2Vec2(gravity.x, gravity.y));
	}

	void SetGravity(const float x, const float y) {
		m_physics->SetGravity(b2Vec2(x, y));
	}

	void SetGravityX(const float x) {
		m_physics->SetGravity(b2Vec2(x, m_physics->GetGravity().y));
	}

	void SetGravityY(const float y) {
		m_physics->SetGravity(b2Vec2(m_physics->GetGravity().x, y));
	}

	glm::vec2 GetGravity() {
		b2Vec2 g = m_physics->GetGravity();
		m_gravity.x = g.x;
		m_gravity.y = g.y;
		return m_gravity;
	}

	float GetGravityX() {
		return m_physics->GetGravity().x;
	}

	float GetGravityY() {
		return m_physics->GetGravity().y;
	}
}

namespace internal::physics {

	b2World* m_physics{ new b2World(b2Vec2(0.f, -10.f)) };
	glm::vec2 m_gravity{ 0.f, 0.f };

	void init() {
		b2BodyDef groundBodyDef;
		groundBodyDef.position.Set(0.0f, -10.0f);
		b2Body* groundBody = m_physics->CreateBody(&groundBodyDef);
		b2PolygonShape groundBox;
		groundBox.SetAsBox(50.0f, 10.0f);
		groundBody->CreateFixture(&groundBox, 0.0f);

		b2BodyDef bodyDef;
		bodyDef.type = b2_kinematicBody;
		bodyDef.position.Set(0.0f, 400.0f);
		b2Body* body = m_physics->CreateBody(&bodyDef);
		b2PolygonShape dynamicBox;
		dynamicBox.SetAsBox(1.0f, 1.0f);
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &dynamicBox;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.3f;
		body->CreateFixture(&fixtureDef);
		/*
		for (;;) {
			m_physics->Step(1.f / 60.f, 6, 2);
			b2Vec2 position = body->GetPosition();
			float angle = body->GetAngle();
			printf("%4.2f %4.2f %4.2f\n", position.x, position.y, angle);
		}
		*/
	}

	void purge() {
		delete m_physics;
	}
}