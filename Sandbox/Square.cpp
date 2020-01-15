#include "Square.h"

void Square::update(const scene::Scene& parent, const std::vector<GameObject*>& objects, const std::vector<scene::Light*>& lights) {
	rot+=.1;
}


void Square::render(const scene::Scene& parent, const std::vector<GameObject*>& objects, const std::vector<scene::Light*>& lights) const {
	using namespace ezrender;

	Translate(glm::vec3(0, 0, 0));
	Rotate(glm::vec3(0, 0, rot));
	Scale(glm::vec2(150, 150));
	Shape(SQUARE);
	Color(glm::vec3(1, .5, .5));
	Mode(FILL);
	Render(parent, objects, lights);
}
