#pragma once

#include "TypedefsAndConstants.hpp"

class ImGuiRenderCommand {
public:
	virtual void operator() (void) = 0;
	operator ImGuiFunction() const;
};
