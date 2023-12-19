#include <Editor/NewProjectModal.hpp>

#include <Editor/GUI.hpp>
#include <Editor/FileDialog.hpp>

#include <Utility/trim.hpp>

NewProjectModal::NewProjectModal(GUI& gui) noexcept :
	gui(gui) {};

void NewProjectModal::Show() const {
	isModalActive = true;
	if (gui.get().HasOpenProject() /* && gui.get().HasUnsavedChanges()*/) {
		stage = Stage::Warning;
	} else {
		stage = Stage::FolderSelection;
	}
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

	const char* title;
	if (stage == Stage::Warning) {
		title = WARNING_TITLE_TEXT;
	} else {
		title = TITLE_TEXT;
	}

	if (isModalActive) {
		ImGui::OpenPopup(title);
		isModalOpen = true;
	}

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal(title, &isModalOpen, ImGuiWindowFlags_AlwaysAutoResize)) {
		switch (stage) {
		using enum NewProjectModal::Stage;
		case Warning:
			RenderWarningDialog();
			break;
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

void NewProjectModal::RenderWarningDialog() {
	ImGui::Text(WARNING_TEXT);
	ImGui::Separator();

	if (ImGui::Button(CONT_NO_SAVE_BUTTON_TEXT, MODAL_BIG_BUTTONS_SIZE)) {
		stage = Stage::FolderSelection;
	}

	ImGui::SameLine();
	if (ImGui::Button(CONT_SAVE_BUTTON_TEXT, MODAL_BUTTONS_SIZE)) {
		gui.get().SaveProjectToDisk();
		stage = Stage::FolderSelection;
	}

	ImGui::SetItemDefaultFocus();
	ImGui::SameLine();
	if (ImGui::Button(NO_BUTTON_TEXT, MODAL_BUTTONS_SIZE)) {
		Hide();
	}
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
	/*
	const char* path = tinyfd_selectFolderDialog("Select a folder for New Project", "");
	if (path) {
		const char* name = nullptr;
		bool badName = true;
		while (badName) {
			name = tinyfd_inputBox("Enter a name for the New Project", "Enter a name for the New Project", "New Project");
			badName = name == nullptr || std::string(name).empty();
			if (badName) {
				tinyfd_messageBox("Warning", "Projects cannot be unnamed.", "ok", "warning", 1);
			}
		}
		gui.CloseProject();
		gui.CreateNewProject(path, name);
		gui.SaveProjectToDisk();
		DOA_LOG_INFO("Succesfully created a new project named %s at %s", name, name);
	}
	*/
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
		GUI& gui = this->gui.get();
		gui.CloseProject();
		gui.CreateNewProject(newProjectData.path, newProjectData.name);
		gui.SaveProjectToDisk();
		DOA_LOG_INFO("Succesfully created a new project named %s at %s", newProjectData.name.c_str(), newProjectData.path.c_str());
		Hide();
	}

	ImGui::SetItemDefaultFocus();
	ImGui::SameLine();
	if (ImGui::Button(NO_BUTTON_TEXT, MODAL_BUTTONS_SIZE)) {
		Hide();
	}
}