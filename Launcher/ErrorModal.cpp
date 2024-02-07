#include <Launcher/ErrorModal.hpp>

#include <Launcher/GUI.hpp>

ErrorModal::ErrorModal(GUI& gui) noexcept :
	gui(gui) {};

void ErrorModal::Show(std::string_view errorText) const {
	isModalActive = true;
	this->errorText = errorText;
}

void ErrorModal::Hide() const {
	ImGui::CloseCurrentPopup();
	isModalActive = false;
}

void ErrorModal::Render() {
	ImGui::PushID("error_modal");

	static constexpr const char* title = TITLE_TEXT;

	if (isModalActive) {
		ImGui::OpenPopup(title);
		isModalOpen = true;
	}

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal(title, &isModalOpen, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::TextUnformatted(errorText.c_str());

		ImGui::Separator();

		if (ImGui::Button(OK_BUTTON_TEXT, { ImGui::GetContentRegionAvail().x, 0 })) {
			Hide();
		}
		ImGui::EndPopup();
	} else {
		Hide(); // NOTE if modals misbehave in the future, look here
	}
	ImGui::PopID();
}
