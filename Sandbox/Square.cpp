#include "Square.h"

Square::Square() {
	doa::text::CreateANSIFont("arial", "C:\\Users\\Doga\\Desktop\\arial.ttf");
	doa::texture::CreateTexture("pencil", "Textures\\pencil.png");
	t = new doa::text::Text(doa::text::Get("arial"), "test TEXT :D :) :P");
	t2 = new doa::text::Text(doa::text::Get("arial"), "WASD to move, Mousewheel to zoom");
	t->SetFixed(false);
	doa::camera::zoom_enabled = true;
	doa::mouse::max_z = 1000;
}

Square::~Square() {
	delete t;
	delete t2;
}

void Square::update(const scene::Scene& parent, const std::vector<GameObject*>& objects, const std::vector<scene::Light*>& lights) {
	static bool dir = false;
	if (dir) {
		textGlow += .001f;
	} else {
		textGlow -= .001f;
	}
	if (textGlow > .5f) {
		dir = false;
	}
	if (textGlow < .1f) {
		dir = true;
	}
	rot += .1f;
}

void Square::render(const scene::Scene& parent, const std::vector<GameObject*>& objects, const std::vector<scene::Light*>& lights) const {
	using namespace ezrender;
	Color(1, 0, 0);
	TranslateX(-200);
	Scale(150, 150);
	RotateZ(-rot);
	Render(parent, objects, lights);

	Reset();
	Texture(doa::texture::Get("pencil"));
	TranslateX(200);
	Scale(150, 150);
	RotateZ(rot);
	Render(parent, objects, lights);

	t->SetTextColor(.4f, .4f, .4f);
	t->SetOutlineColor(.4f, .9f, .6f);
	t->SetCharBorderWidth(textGlow);
	t->SetScale(2.2, 2.2);
	t->SetReferencePoint(doa::text::Text::CENTER);
	t->SetPosition(0, 220, 0);
	t->render(parent, objects, lights);

	t2->SetTextColor(.4f, .4f, .4f);
	t2->SetOutlineColor(.9f, .4f, .6f);
	t2->SetCharBorderWidth(.6f - textGlow);
	t2->SetScale(2.2, 2.2);
	t2->SetReferencePoint(doa::text::Text::CENTER);
	t2->SetPosition(0, -220, 1);
	t2->render(parent, objects, lights);
}