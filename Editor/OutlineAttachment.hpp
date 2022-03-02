#pragma once

#include <Core.hpp>
#include <Project.hpp>

#include "GUI.hpp"

struct OutlineAttachment {

	std::shared_ptr<GUI> gui;
	glm::vec4 OutlineColor{ 1.0f, 1.0f, 1.0f, 1.0f };

	inline OutlineAttachment(std::shared_ptr<GUI> gui) :
		gui(gui) {}

	inline void BeforeFrame(Project* project) {}
	inline void AfterFrame(Project* project) {
		if (project == nullptr || !project->HasOpenScene()) { return; }
		if (gui->sh.selectedEntity == NULL_ENTT) { return; }

		Scene& scene = project->GetOpenScene();
		DOA_LOG_INFO("selected entt: %d", gui->sh.selectedEntity);
		/*
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);




		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00); // disable writing to the stencil buffer
		glDisable(GL_DEPTH_TEST);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);*/
	}
};

