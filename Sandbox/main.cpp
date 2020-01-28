#include "doa.h"
#include "Square.h"
#include <chrono>

using namespace doa;

int main() {
	Init();
	Window *window{ doa::CreateWindow("DoaWindow", 960, 540, false) };

	scene::Scene *scene{ new scene::Scene("alt") };

	scene::ACTIVE_SCENE = scene;

	Square sq;
	scene->Add(&sq);

	Loop();
	Purge();

	return 0;
}