#include <Editor/NewAssetModal.hpp>

#include <format>

#include <Engine/ShaderProgramSerializer.hpp>

#include <Editor/GUI.hpp>
#include <Editor/CodeGenerator.hpp>
#include <Editor/ImGuiExtensions.hpp>

#include <Utility/Trim.hpp>

NewAssetModal::NewAssetModal(GUI& gui) noexcept :
	gui(gui) {};

void NewAssetModal::ShowSceneCreationModal(FNode& currentFolder) const {
	/* cast-away const - this modal is never created const */
	const_cast<NewAssetModal*>(this)->Reset(currentFolder, NewAssetData::AssetType::Scene);
}
void NewAssetModal::ShowComponentCreationModal(FNode& currentFolder) const {
	/* cast-away const - this modal is never created const */
	const_cast<NewAssetModal*>(this)->Reset(currentFolder, NewAssetData::AssetType::Component);
}
void NewAssetModal::ShowVertexShaderCreationModal(FNode& currentFolder) const {
	/* cast-away const - this modal is never created const */
	const_cast<NewAssetModal*>(this)->Reset(currentFolder, NewAssetData::AssetType::VertexShader);
}
void NewAssetModal::ShowTessellationControlShaderCreationModal(FNode& currentFolder) const {
	/* cast-away const - this modal is never created const */
	const_cast<NewAssetModal*>(this)->Reset(currentFolder, NewAssetData::AssetType::TessellationControlShader);
}
void NewAssetModal::ShowTessellationEvaluationShaderCreationModal(FNode& currentFolder) const {
	/* cast-away const - this modal is never created const */
	const_cast<NewAssetModal*>(this)->Reset(currentFolder, NewAssetData::AssetType::TessellationEvaluationShader);
}
void NewAssetModal::ShowGeometryShaderCreationModal(FNode& currentFolder) const {
	/* cast-away const - this modal is never created const */
	const_cast<NewAssetModal*>(this)->Reset(currentFolder, NewAssetData::AssetType::GeometryShader);
}
void NewAssetModal::ShowFragmentShaderCreationModal(FNode& currentFolder) const {
	/* cast-away const - this modal is never created const */
	const_cast<NewAssetModal*>(this)->Reset(currentFolder, NewAssetData::AssetType::FragmentShader);
}
void NewAssetModal::ShowShaderProgramCreationModal(FNode& currentFolder) const {
	/* cast-away const - this modal is never created const */
	const_cast<NewAssetModal*>(this)->Reset(currentFolder, NewAssetData::AssetType::ShaderProgram);
}

void NewAssetModal::Hide() const {
	ImGui::CloseCurrentPopup();
	isModalActive = false;
}

void NewAssetModal::Reset(FNode& currentFolder, NewAssetData::AssetType typeOfAssetToCreate) {
	isModalActive = true;
	stage = Stage::NameSelection;
	std::memset(buf.data(), '\0', buf.size());
	badName = false;

	newAssetData.type = typeOfAssetToCreate;
	newAssetData.currentFolder = &currentFolder;
	newAssetData.path = std::string("ROOT")
		.append(1, static_cast<char>(std::filesystem::path::preferred_separator))
		.append(currentFolder.Path().string()).c_str();

	static auto defSceneName          = "MyScene";                        assert(std::strlen(defSceneName)          < buf.size());
	static auto defComponentName	  = "MyComponent";                    assert(std::strlen(defComponentName)      < buf.size());
	static auto defVertexShaderName	  = "MyVertexShader";                 assert(std::strlen(defVertexShaderName)   < buf.size());
	static auto defTessCtrlShaderName = "MyTessellationControlShader";    assert(std::strlen(defTessCtrlShaderName) < buf.size());
	static auto defTessEvalShaderName = "MyTessellationEvaluationShader"; assert(std::strlen(defTessEvalShaderName) < buf.size());
	static auto defGeometryShaderName = "MyGeometryShader";               assert(std::strlen(defGeometryShaderName) < buf.size());
	static auto defFragmentShaderName = "MyFragmentShader";               assert(std::strlen(defFragmentShaderName) < buf.size());
	static auto defShaderProgramName  = "MyShaderProgram";                assert(std::strlen(defShaderProgramName)  < buf.size());
	switch(typeOfAssetToCreate) {
		using enum NewAssetData::AssetType;
	case Scene:
		std::strcpy(buf.data(), defSceneName);
		titleText = std::format(TITLE_TEXT, "Scene");
		confirmText = std::format(CONFIRM_TEXT, "a Scene");
		break;
	case Component:
		std::strcpy(buf.data(), defComponentName);
		titleText = std::format(TITLE_TEXT, "Component");
		confirmText = std::format(CONFIRM_TEXT, "a Component");
		break;
	case VertexShader:
		std::strcpy(buf.data(), defVertexShaderName);
		titleText = std::format(TITLE_TEXT, "Vertex Shader");
		confirmText = std::format(CONFIRM_TEXT, "a Vertex Shader");
		break;
	case TessellationControlShader:
		std::strcpy(buf.data(), defTessCtrlShaderName);
		titleText = std::format(TITLE_TEXT, "Tessellation Control Shader");
		confirmText = std::format(CONFIRM_TEXT, "a Tessellation Control Shader");
		break;
	case TessellationEvaluationShader:
		std::strcpy(buf.data(), defTessEvalShaderName);
		titleText = std::format(TITLE_TEXT, "Tessellation Evaluation Shader");
		confirmText = std::format(CONFIRM_TEXT, "a Tessellation Evaluation Shader");
		break;
	case GeometryShader:
		std::strcpy(buf.data(), defGeometryShaderName);
		titleText = std::format(TITLE_TEXT, "Geometry Shader");
		confirmText = std::format(CONFIRM_TEXT, "a Geometry Shader");
		break;
	case FragmentShader:
		std::strcpy(buf.data(), defFragmentShaderName);
		titleText = std::format(TITLE_TEXT, "Fragment Shader");
		confirmText = std::format(CONFIRM_TEXT, "a Fragment Shader");
		break;
	case ShaderProgram:
		std::strcpy(buf.data(), defShaderProgramName);
		titleText = std::format(TITLE_TEXT, "Shader Program");
		confirmText = std::format(CONFIRM_TEXT, "a Shader Program");
		break;
	case Texture:
		break;
	case Model:
		break;
	default:
		assert(false); // invalid enum
	}
}

void NewAssetModal::Render() {
	ImGui::PushID("new_component_asset_modal");

	if (isModalActive) {
		ImGui::OpenPopup(titleText.c_str());
		isModalOpen = true;
	}

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal(titleText.c_str(), &isModalOpen, ImGuiWindowFlags_AlwaysAutoResize)) {
		switch (stage) {
			using enum NewAssetModal::Stage;
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
void NewAssetModal::RenderNameSelectionDialog() {
	bool create = false;
	if (badName) {
		ImGui::PushStyleColor(ImGuiCol_Border, { 1.0f, 0.0f, 0.0f, 1.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
	}
	ImGuiInputTextFlags flags = ImGuiInputTextFlags_AutoSelectAll |
								ImGuiInputTextFlags_CharsNoBlank |
								ImGuiInputTextFlags_EnterReturnsTrue;
	FocusNextItem();
	if (ImGui::InputTextWithHint("###project_name", "Asset Name", buf.data(), buf.size(), flags)) {
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
		newAssetData.name = buf.data();
		trim(newAssetData.name);
		if (!newAssetData.name.empty()) {
			// sanitize extension from name
			newAssetData.name = std::filesystem::path(newAssetData.name).stem().string();
			stage = Stage::Confirmation;
		} else {
			badName = true;
		}
	}
}
void NewAssetModal::RenderConfirmationDialog() {
	ImGui::Text(("Name: " + newAssetData.name).c_str());
	ImGui::Text(("Path: " + newAssetData.path).c_str());
	ImGui::NewLine();
	ImGui::Text(confirmText.c_str());
	ImGui::Separator();

	if (ImGui::Button(YES_BUTTON_TEXT, MODAL_BUTTONS_SIZE)) {
		CreateAsset();
		Hide();
	}

	ImGui::SetItemDefaultFocus();
	ImGui::SameLine();
	if (ImGui::Button(NO_BUTTON_TEXT, MODAL_BUTTONS_SIZE)) {
		Hide();
	}
}

void NewAssetModal::CreateAsset() {
	switch (newAssetData.type) {
		using enum NewAssetData::AssetType;
	case Scene:
		CreateSceneAsset();
		break;
	case Component:
		CreateComponentAsset();
		break;
	case VertexShader:
		CreateVertexShaderAsset();
		break;
	case TessellationControlShader:
		CreateTessellationControlAsset();
		break;
	case TessellationEvaluationShader:
		CreateTessellationEvaluationAsset();
		break;
	case GeometryShader:
		CreateGeometryShaderAsset();
		break;
	case FragmentShader:
		CreateFragmentShaderAsset();
		break;
	case ShaderProgram:
		CreateShaderProgramAsset();
		break;
	case Texture:
		break;
	case Model:
		break;
	default:
		assert(false); // invalid enum
	}
}
void NewAssetModal::CreateSceneAsset() {
	GUI& gui = this->gui.get();
	gui.CreateNewScene(*newAssetData.currentFolder, newAssetData.name);
}
void NewAssetModal::CreateComponentAsset() {
	const GUI& gui = this->gui.get();
	const auto sourceCode = CodeGenerator::GenerateComponentDeclaration(newAssetData.name);
	gui.CORE->GetAssets()->CreateAssetAt<Component>(*newAssetData.currentFolder, newAssetData.name + Assets::COMP_EXT, sourceCode);
	DOA_LOG_INFO("Succesfully created a new component asset named %s at %s", newAssetData.name.c_str(), newAssetData.path.c_str());
}
void NewAssetModal::CreateVertexShaderAsset() {
	const GUI& gui = this->gui.get();
	const auto sourceCode = CodeGenerator::GenerateVertexShaderCode();
	gui.CORE->GetAssets()->CreateAssetAt<Shader>(*newAssetData.currentFolder, newAssetData.name + Assets::VERTEX_SHADER_EXT, sourceCode);
	DOA_LOG_INFO("Succesfully created a new vertex shader asset named %s at %s", newAssetData.name.c_str(), newAssetData.path.c_str());
}
void NewAssetModal::CreateTessellationControlAsset() {
	const GUI& gui = this->gui.get();
	const auto sourceCode = CodeGenerator::GenerateTessellationControlShaderCode();
	gui.CORE->GetAssets()->CreateAssetAt<Shader>(*newAssetData.currentFolder, newAssetData.name + Assets::TESS_CTRL_SHADER_EXT, sourceCode);
	DOA_LOG_INFO("Succesfully created a new tessellation control shader asset named %s at %s", newAssetData.name.c_str(), newAssetData.path.c_str());
}
void NewAssetModal::CreateTessellationEvaluationAsset() {
	const GUI& gui = this->gui.get();
	const auto sourceCode = CodeGenerator::GenerateTessellationEvaluationShaderCode();
	gui.CORE->GetAssets()->CreateAssetAt<Shader>(*newAssetData.currentFolder, newAssetData.name + Assets::TESS_EVAL_SHADER_EXT, sourceCode);
	DOA_LOG_INFO("Succesfully created a new tessellation evaluation shader asset named %s at %s", newAssetData.name.c_str(), newAssetData.path.c_str());
}
void NewAssetModal::CreateGeometryShaderAsset() {
	const GUI& gui = this->gui.get();
	const auto sourceCode = CodeGenerator::GenerateGeometryShaderCode();
	gui.CORE->GetAssets()->CreateAssetAt<Shader>(*newAssetData.currentFolder, newAssetData.name + Assets::GEOMETRY_SHADER_EXT, sourceCode);
	DOA_LOG_INFO("Succesfully created a new geometry shader asset named %s at %s", newAssetData.name.c_str(), newAssetData.path.c_str());
}
void NewAssetModal::CreateFragmentShaderAsset() {
	const GUI& gui = this->gui.get();
	const auto sourceCode = CodeGenerator::GenerateFragmentShaderCode();
	gui.CORE->GetAssets()->CreateAssetAt<Shader>(*newAssetData.currentFolder, newAssetData.name + Assets::FRAGMENT_SHADER_EXT, sourceCode);
	DOA_LOG_INFO("Succesfully created a new fragment shader asset named %s at %s", newAssetData.name.c_str(), newAssetData.path.c_str());
}
void NewAssetModal::CreateShaderProgramAsset() {
	const GUI& gui = this->gui.get();
	ShaderProgram temporary{ .Name = newAssetData.name };
	const auto data = temporary.Serialize();
	gui.CORE->GetAssets()->CreateAssetAt<ShaderProgram>(*newAssetData.currentFolder, newAssetData.name + Assets::SHADER_PROGRAM_EXT, data);
	DOA_LOG_INFO("Succesfully created a new shader program asset named %s at %s", newAssetData.name.c_str(), newAssetData.path.c_str());
}