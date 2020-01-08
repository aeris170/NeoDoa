#pragma once
#include "doa.h"

using namespace doa;

class Square : public scene::GameObject
{
public:
	glm::vec3 color;

	void render(const scene::Scene& parent, const std::vector<GameObject*>* const objects, const std::vector<scene::Light*>* const lights) const override;

};

