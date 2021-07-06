#pragma once

#include "TypedefsAndConstants.hpp"

class ImGuiRenderCommand {
public:
	virtual void operator() (float delta) = 0;
};
