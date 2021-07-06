#pragma once

#include "ImGuiRenderCommand.hpp"
#include "Project.hpp"

struct Texture;

class Editor : public ImGuiRenderCommand {
private:
	std::weak_ptr<Texture> neodoaBanner;

public:
	std::shared_ptr<Project> _openProject;

	Editor() noexcept;

	void operator() (float delta) override;
};
