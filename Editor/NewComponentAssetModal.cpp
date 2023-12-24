#include <Editor/NewComponentAssetModal.hpp>

#include <Editor/GUI.hpp>
#include <Editor/CodeGenerator.hpp>
#include <Editor/ImGuiExtensions.hpp>

#include <Utility/trim.hpp>

NewComponentAssetModal::NewComponentAssetModal(GUI& gui) noexcept :
	gui(gui) {};

void NewComponentAssetModal::Show(FNode& currentFolder) const {
	this->currentFolder = &currentFolder;
	isModalActive = true;
	stage = Stage::NameSelection;
	newComponentAssetData.path = std::string("ROOT").append(sizeof(char), static_cast<char>(std::filesystem::path::preferred_separator)).append(currentFolder.Path().string()).c_str();
	std::memset(buf.data(), '\0', buf.size());
	static auto defCompName = "MyComponent";
	assert(std::strlen(defCompName) < buf.size());
	std::strcpy(buf.data(), "MyComponent");
	badName = false;
}

void NewComponentAssetModal::Hide() const {
	ImGui::CloseCurrentPopup();
	isModalActive = false;
}

void NewComponentAssetModal::Render() {
	ImGui::PushID("new_component_asset_modal");

	if (isModalActive) {
		ImGui::OpenPopup(TITLE_TEXT);
		isModalOpen = true;
	}

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal(TITLE_TEXT, &isModalOpen, ImGuiWindowFlags_AlwaysAutoResize)) {
		switch (stage) {
			using enum NewComponentAssetModal::Stage;
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
void NewComponentAssetModal::RenderNameSelectionDialog() {
	bool create = false;
	if (badName) {
		ImGui::PushStyleColor(ImGuiCol_Border, { 1.0f, 0.0f, 0.0f, 1.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
	}
	ImGuiInputTextFlags flags = ImGuiInputTextFlags_AutoSelectAll |
								ImGuiInputTextFlags_CharsNoBlank |
								ImGuiInputTextFlags_EnterReturnsTrue;
	FocusNextItem();
	if (ImGui::InputTextWithHint("###project_name", "Component Asset Name", buf.data(), buf.size(), flags)) {
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
		newComponentAssetData.name = buf.data();
		trim(newComponentAssetData.name);
		if (!newComponentAssetData.name.empty()) {
			const std::filesystem::path filePath = newComponentAssetData.name;
			if (filePath.extension() != Assets::COMP_EXT) {
				newComponentAssetData.name.append(Assets::COMP_EXT);
			}
			stage = Stage::Confirmation;
		} else {
			badName = true;
		}
	}
}
void NewComponentAssetModal::RenderConfirmationDialog() {
	ImGui::Text(("Name: " + newComponentAssetData.name).c_str());
	ImGui::Text(("Path: " + newComponentAssetData.path).c_str());
	ImGui::NewLine();
	ImGui::Text("Continue and create component asset?");
	ImGui::Separator();

	if (ImGui::Button(YES_BUTTON_TEXT, MODAL_BUTTONS_SIZE)) {
		GUI& gui = this->gui.get();
		const auto componentSourceCode = CodeGenerator::GenerateComponentDeclaration(newComponentAssetData.name);
		auto component = Component::Deserialize(componentSourceCode);
		gui.CORE->Assets()->CreateAssetAt<Component>(*currentFolder, newComponentAssetData.name, std::move(component));
		DOA_LOG_INFO("Succesfully created a new component asset named %s at %s", newComponentAssetData.name.c_str(), newComponentAssetData.path.c_str());
		Hide();
	}

	ImGui::SetItemDefaultFocus();
	ImGui::SameLine();
	if (ImGui::Button(NO_BUTTON_TEXT, MODAL_BUTTONS_SIZE)) {
		Hide();
	}
}