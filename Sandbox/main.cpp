#include "doa.h"
#include "Square.h"

using namespace doa;

int main() {
	Init();

	CreateWindow("DoaWindow", 1920, 1080);

	scene::Scene *scene{ new scene::Scene("alt") };

	scene->SetName("hello");

	scene::ACTIVE_SCENE = scene::SCENES["hello"];

	Square sq;
	scene->Add(&sq);

	Loop();
	Purge();

	return 0;
}