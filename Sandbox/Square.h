#pragma once
#include "doa.h"

using namespace doa;

class Square : public scene::GameObject {
private:
	float rot = 0;

public:
	void update(const scene::Scene& parent, const std::vector<GameObject*>& objects, const std::vector<scene::Light*>& lights) override;
	void render(const scene::Scene& parent, const std::vector<GameObject*>& objects, const std::vector<scene::Light*>& lights) const override;
};
