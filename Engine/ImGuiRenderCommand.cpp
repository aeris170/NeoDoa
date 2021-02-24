#include "ImGuiRenderCommand.hpp"

ImGuiRenderCommand::operator ImGuiFunction() const {
	return *this;
}
