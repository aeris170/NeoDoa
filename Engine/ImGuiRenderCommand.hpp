#pragma once

class ImGuiRenderCommand {
public:
	virtual void operator() (float delta) = 0;
};
