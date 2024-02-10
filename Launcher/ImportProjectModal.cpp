#include <Launcher/ImportProjectModal.hpp>

#include <Engine/ProjectDeserializer.hpp>

#include <Launcher/GUI.hpp>
#include <Launcher/FileDialog.hpp>

ImportProjectModal::ImportProjectModal(GUI& gui) noexcept :
	gui(gui) {};

void ImportProjectModal::Show() const noexcept {
	isModalActive = true;
	stage = Stage::Selection;
	isFileDialogOpen = false;
}
void ImportProjectModal::Hide() const noexcept {
	ImGui::CloseCurrentPopup();
	isModalActive = false;
}
bool ImportProjectModal::IsVisible() const noexcept { return isModalOpen; }

void ImportProjectModal::Render() noexcept {
	ImGui::PushID("open_project_modal");

	static constexpr const char* title = TITLE_TEXT;

	if (isModalActive) {
		ImGui::OpenPopup(title);
		isModalOpen = true;
	}

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal(title, &isModalOpen, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings)) {
		switch (stage) {
		using enum ImportProjectModal::Stage;
		case Selection:
			ImGui::Text("Waiting for selection...");
			RenderSelectionDialog();
			break;
		case Confirmation:
			RenderConfirmationDialog();
			break;
		}
		ImGui::EndPopup();
	} else {
		Hide();
	}
	ImGui::PopID();
}
void ImportProjectModal::RenderSelectionDialog() noexcept {
	if (!isFileDialogOpen) {
		FileDialog::Instance().Open("ImportProjectSelectionDialog", "Select Project File", "NeoDoa Project Files (*.doa){.doa},.*");
		isFileDialogOpen = true;
	}
	if (FileDialog::Instance().IsDone("ImportProjectSelectionDialog")) {
		if (FileDialog::Instance().HasResult()) {
			assert(!FileDialog::Instance().GetResult().string().empty());
			std::filesystem::path path{ FileDialog::Instance().GetResult() };
			importProjectData.path = path.parent_path().string();
			if (!importProjectData.path.ends_with(static_cast<char>(std::filesystem::path::preferred_separator))) {
				importProjectData.path.append(1, static_cast<char>(std::filesystem::path::preferred_separator));
			}
			importProjectData.name = path.stem().string();
			importProjectData.fullPath = path.string();
			stage = Stage::Confirmation;
		} else {
			Hide();
		}
		FileDialog::Instance().Close();
	}
}
void ImportProjectModal::RenderConfirmationDialog() noexcept {
	ImGui::Text("Name: %s", importProjectData.name.c_str());
	ImGui::Text("Path: %s", importProjectData.path.c_str());
	ImGui::NewLine();
	ImGui::TextUnformatted("Continue and open project?");
	ImGui::Separator();

	if (ImGui::Button(YES_BUTTON_TEXT, MODAL_BUTTONS_SIZE)) {
		GUI& gui = this->gui.get();
		FNode file({ .name = importProjectData.fullPath });
		auto pdr = DeserializeProject(&file);
		gui.InsertProjectData({ !pdr.erred, false, importProjectData.name, importProjectData.path, {} });
		Hide();
	}

	ImGui::SetItemDefaultFocus();
	ImGui::SameLine();
	if (ImGui::Button(NO_BUTTON_TEXT, MODAL_BUTTONS_SIZE)) {
		Hide();
	}
}