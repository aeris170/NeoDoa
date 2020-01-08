#include "doa.h"

using namespace doa;

int main() {
	Init();
	Window *window{ doa::CreateWindow("DoaWindow", 960, 540, false) };

	texture::Texture* brick{ texture::CreateTexture("testTex", "Textures/brick.png") };
	texture::Texture* pencil{ texture::CreateTexture("", "Textures/pencil.png") };
	texture::Texture* lightTest{ texture::CreateTexture("t", "Textures/lighttest.jpg") };

	primitives::Primitive t{ primitives::TRIANGLE, primitives::DRAW, primitives::COLOR };
	t.SetVertexColor(0, glm::vec3(0, 0, 1));
	t.SetPosition(-250, -150, 0);
	t.SetScale(100, 100);

	primitives::Primitive s{ primitives::CIRCLE, primitives::FILL, primitives::COLOR };
	s.SetAllVerticesColors(glm::vec3(.7, .4, .7));
	s.SetVertexColor(0, glm::vec3(0, 0, 0));
	s.SetPosition(0, -150, 0);
	s.SetScale(100, 100);

	primitives::Primitive p{ primitives::HEXAGON, primitives::FILL, primitives::COLOR };
	p.SetVertexColor(0, glm::vec3(0, 1, 0));
	p.SetPosition(250, -150, 0);
	p.SetScale(100, 100);


	primitives::Primitive r1{ primitives::HEXAGON, primitives::FILL, primitives::COLOR };
	r1.SetVertexColor(0, glm::vec3(.5, .5, .5));
	r1.SetVertexColor(1, glm::vec3(1, 0, 0));
	r1.SetVertexColor(2, glm::vec3(1, 1, 0));
	r1.SetVertexColor(3, glm::vec3(0, 1, 0));
	r1.SetVertexColor(4, glm::vec3(0, 1, 1));
	r1.SetVertexColor(5, glm::vec3(0, 0, 1));
	r1.SetVertexColor(6, glm::vec3(1, 0, 1));
	r1.SetPosition(-250, 150, 0);
	r1.SetScale(100, 100);

	primitives::Primitive r2{ primitives::CIRCLE, primitives::FILL, primitives::TEXTURE };
	r2.SetVertexColor(0, glm::vec3(1, 1, 1));
	r2.SetVertexColor(1, glm::vec3(1, 0, 0));
	r2.SetVertexColor(2, glm::vec3(0, 1, 0));
	r2.SetVertexColor(3, glm::vec3(0, 0, 1));
	r2.SetVertexColor(4, glm::vec3(0, 1, 1));
	r2.SetPosition(0, 150, 0);
	r2.SetScale(100, 100);
	r2.SetTexture(brick);

	primitives::Primitive r3{ primitives::SQUARE, primitives::FILL, primitives::TEXTURE };
	r3.SetVertexColor(0, glm::vec3(1, 1, 1));
	r3.SetVertexColor(1, glm::vec3(1, 0, 0));
	r3.SetVertexColor(2, glm::vec3(0, 1, 0));
	r3.SetVertexColor(3, glm::vec3(0, 0, 1));
	r3.SetVertexColor(4, glm::vec3(0, 1, 1));
	r3.SetPosition(0, 0, 1000);
	r3.SetScale(480, 270);
	r3.SetTexture(lightTest);

	scene::Scene *scene{ new scene::Scene("alt") };

	scene::Scene *scene2{ new scene::Scene("üst") };

	scene->Add(&t);
	scene->Add(&s);
	scene->Add(&p);
	scene->Add(&r1);
	scene->Add(&r2);

	scene2->Add(&r3);

	scene2->SetAmbientLight(.06f, .06f, .1f);

	scene::PointLight light{ glm::vec3(145, 155, 1000), glm::vec3(1, 1, 1), 100 };
	light.SetRadius(300);

	scene2->Add(&light);

	scene::ACTIVE_SCENE = scene2;

	scene2->SetName("aa");


	std::vector<texture::Texture*> frames;
	frames.push_back(pencil);
	frames.push_back(brick);

	animation::Animation* a{ animation::CreateAnimation("testAnim", frames, std::chrono::seconds(3)) };
	r3.SetDisplayMode(primitives::COLOR_OVER_ANIMATION);
	r3.SetAnimation(a);

	Loop();
	Purge();
	return 0;
}