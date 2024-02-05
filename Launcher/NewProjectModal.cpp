#include <Launcher/NewProjectModal.hpp>

#include <Utility/Trim.hpp>

#include <Launcher/GUI.hpp>
#include <Launcher/FileDialog.hpp>

NewProjectModal::NewProjectModal(GUI& gui) noexcept :
	gui(gui) {};

void NewProjectModal::Show() const {
	isModalActive = true;
	stage = Stage::FolderSelection;
	isFileDialogOpen = false;
	newProjectData = {};
	std::memset(buf.data(), '\0', buf.size());
	badName = false;
}

void NewProjectModal::Hide() const {
	ImGui::CloseCurrentPopup();
	isModalActive = false;
}

void NewProjectModal::Render() {
	ImGui::PushID("new_project_modal");

	static constexpr const char* title = TITLE_TEXT;

	if (isModalActive) {
		ImGui::OpenPopup(title);
		isModalOpen = true;
	}

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal(title, &isModalOpen, ImGuiWindowFlags_AlwaysAutoResize)) {
		switch (stage) {
		using enum NewProjectModal::Stage;
		case FolderSelection:
			RenderFolderSelectionDialog();
			break;
		case NameSelection:
			RenderNameSelectionDialog();
			break;
		case Confirmation:
			RenderConfirmationDialog();
			break;
		}
		ImGui::EndPopup();
	} else {
		Hide(); // NOTE if modals misbehave in the future, look here
	}
	ImGui::PopID();
}
void NewProjectModal::RenderFolderSelectionDialog() {
	if (!isFileDialogOpen) {
		FileDialog::Instance().Open("NewProjectFolderSelectionDialog", "Select Project Root Folder", "");
		isFileDialogOpen = true;
	}
	if (FileDialog::Instance().IsDone("NewProjectFolderSelectionDialog")) {
		if (FileDialog::Instance().HasResult()) {
			assert(!FileDialog::Instance().GetResult().string().empty());
			newProjectData.path = FileDialog::Instance().GetResult().string();
			stage = Stage::NameSelection;
		} else {
			Hide();
		}
		FileDialog::Instance().Close();
	}
}
void NewProjectModal::RenderNameSelectionDialog() {
	bool create = false;
	if (badName) {
		ImGui::PushStyleColor(ImGuiCol_Border, { 1.0f, 0.0f, 0.0f, 1.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
	}
	if (ImGui::InputTextWithHint("###project_name", "Project Name", buf.data(), buf.size(), ImGuiInputTextFlags_EnterReturnsTrue)) {
		create = true;
	}
	if (badName) {
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
	}
	ImGui::Separator();

	if (ImGui::Button(OK_BUTTON_TEXT, MODAL_BUTTONS_SIZE)) {
		create = true;
	}

	ImGui::SetItemDefaultFocus();
	ImGui::SameLine();
	if (ImGui::Button(CANCEL_BUTTON_TEXT, MODAL_BUTTONS_SIZE)) {
		Hide();
	}

	if (create) {
		newProjectData.name = buf.data();
		trim(newProjectData.name);
		if (!newProjectData.name.empty()) {
			stage = Stage::Confirmation;
		} else {
			badName = true;
		}
	}
}
void NewProjectModal::RenderConfirmationDialog() {
	ImGui::Text(("Name: " + newProjectData.name).c_str());
	ImGui::Text(("Path: " + newProjectData.path).c_str());
	ImGui::NewLine();
	ImGui::Text("Continue and create project?");
	ImGui::Separator();

	if (ImGui::Button(YES_BUTTON_TEXT, MODAL_BUTTONS_SIZE)) {
		static const auto& Core = Core::GetCore();
		GUI& gui = this->gui.get();

		Core->CreateAndLoadProject(newProjectData.path, newProjectData.name);
		Project& project = *Core->LoadedProject().get();
		AssetHandle handle = Core->Assets()->CreateAssetAt<Scene>(Core->Assets()->Root(), "Sample Scene.scn", std::string("Sample Scene"));

		Core->SaveLoadedProjectToDisk();
		gui.InsertProjectData({ true, false, newProjectData.name, newProjectData.path, {} });
		Hide();
	}

	ImGui::SetItemDefaultFocus();
	ImGui::SameLine();
	if (ImGui::Button(NO_BUTTON_TEXT, MODAL_BUTTONS_SIZE)) {
		Hide();
	}
}