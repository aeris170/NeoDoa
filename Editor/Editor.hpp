#pragma once

#include "ImGuiRenderCommand.hpp"

struct Texture;

class Editor : public ImGuiRenderCommand {
private:
	std::weak_ptr<Texture> neodoaBanner;

public:
	Editor() noexcept;
	void operator() (void) override;
};