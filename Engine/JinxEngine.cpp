#include "JinxEngine.hpp"

JinxEngine::JinxEngine() noexcept :
	_runtime(Jinx::CreateRuntime()) {

};
