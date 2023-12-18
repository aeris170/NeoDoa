#include <Editor/OpenProjectModal.hpp>

#include <Editor/GUI.hpp>
#include <Editor/FileDialog.hpp>

OpenProjectModal::OpenProjectModal(GUI& gui) noexcept :
	gui(gui) {};

void OpenProjectModal::Show() const {
	isModalActive = true;
	if (gui.get().HasOpenProject() /* && gui.get().HasUnsavedChanges()*/) {
		stage = Stage::Warning;
	} else {
		stage = Stage::Selection;
	}
	isFileDialogOpen = false;
}

void OpenProjectModal::Hide() const {
	ImGui::CloseCurrentPopup();
	isModalActive = false;
}

void OpenProjectModal::Render() {
	ImGui::PushID("open_project_modal");

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
		using enum OpenProjectModal::Stage;
		case Warning:
			RenderWarningDialog();
			break;
		case Selection:
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

void OpenProjectModal::RenderWarningDialog() {
	ImGui::Text(WARNING_TEXT);
	ImGui::Separator();

	const ImGuiStyle& style = ImGui::GetStyle();

	float size = (MODAL_BUTTONS_SIZE.x + style.ItemSpacing.x) * 2.0f;
	float avail = ImGui::GetWindowSize().x;

	float offset = (avail - size) * 0.5f;
	if (offset > 0.0f) {
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
	}

	if (ImGui::Button(YES_BUTTON_TEXT, MODAL_BUTTONS_SIZE)) {
		stage = Stage::Selection;
	}

	ImGui::SetItemDefaultFocus();
	ImGui::SameLine();
	if (ImGui::Button(NO_BUTTON_TEXT, MODAL_BUTTONS_SIZE)) {
		Hide();
	}
}
void OpenProjectModal::RenderSelectionDialog() {
	if (!isFileDialogOpen) {
		FileDialog::Instance().Open("OpenProjectSelectionDialog", "Select Project File", "NeoDoa Project Files (*.doa){.doa},.*");
		isFileDialogOpen = true;
	}
	if (FileDialog::Instance().IsDone("OpenProjectSelectionDialog")) {
		if (FileDialog::Instance().HasResult()) {
			assert(!FileDialog::Instance().GetResult().string().empty());
			std::filesystem::path path{ FileDialog::Instance().GetResult() };
			openProjectData.path = path.parent_path().string();
			openProjectData.name = path.stem().string();
			openProjectData.fullPath = path.string();
			stage = Stage::Confirmation;
		} else {
			Hide();
		}
		FileDialog::Instance().Close();
	}
}
void OpenProjectModal::RenderConfirmationDialog() {
	ImGui::Text(("Name: " + openProjectData.name).c_str());
	ImGui::Text(("Path: " + openProjectData.path).c_str());
	ImGui::NewLine();
	ImGui::Text("Continue and open project?");
	ImGui::Separator();

	const ImGuiStyle& style = ImGui::GetStyle();

	float size = (MODAL_BUTTONS_SIZE.x + style.ItemSpacing.x) * 2.0f;
	float avail = ImGui::GetWindowSize().x;

	float offset = (avail - size) * 0.5f;
	if (offset > 0.0f) {
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
	}

	if (ImGui::Button(YES_BUTTON_TEXT, MODAL_BUTTONS_SIZE)) {
		GUI& gui = this->gui.get();
		gui.CloseProject();
		gui.OpenProjectFromDisk(openProjectData.fullPath);
		Hide();
	}

	ImGui::SetItemDefaultFocus();
	ImGui::SameLine();
	if (ImGui::Button(NO_BUTTON_TEXT, MODAL_BUTTONS_SIZE)) {
		Hide();
	}
}