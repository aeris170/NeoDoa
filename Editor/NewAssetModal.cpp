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
void NewAssetModal::ShowSamplerCreationModal(FNode& currentFolder) const {
	/* cast-away const - this modal is never created const */
	const_cast<NewAssetModal*>(this)->Reset(currentFolder, NewAssetData::AssetType::Sampler);
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
void NewAssetModal::ShowMaterialCreationModal(FNode& currentFolder) const {
	/* cast-away const - this modal is never created const */
	const_cast<NewAssetModal*>(this)->Reset(currentFolder, NewAssetData::AssetType::Material);
}
void NewAssetModal::ShowFrameBufferCreationModal(FNode& currentFolder) const {
	/* cast-away const - this modal is never created const */
	const_cast<NewAssetModal*>(this)->Reset(currentFolder, NewAssetData::AssetType::FrameBuffer);
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

	static constexpr auto defSceneName          = "MyScene";                        static_assert(std::char_traits<char>::length(defSceneName)          < BufferSize);
	static constexpr auto defComponentName	    = "MyComponent";                    static_assert(std::char_traits<char>::length(defComponentName)      < BufferSize);
	static constexpr auto defSamplerName        = "MySampler";                      static_assert(std::char_traits<char>::length(defSamplerName)        < BufferSize);
	static constexpr auto defVertexShaderName	= "MyVertexShader";                 static_assert(std::char_traits<char>::length(defVertexShaderName)   < BufferSize);
	static constexpr auto defTessCtrlShaderName = "MyTessellationControlShader";    static_assert(std::char_traits<char>::length(defTessCtrlShaderName) < BufferSize);
	static constexpr auto defTessEvalShaderName = "MyTessellationEvaluationShader"; static_assert(std::char_traits<char>::length(defTessEvalShaderName) < BufferSize);
	static constexpr auto defGeometryShaderName = "MyGeometryShader";               static_assert(std::char_traits<char>::length(defGeometryShaderName) < BufferSize);
	static constexpr auto defFragmentShaderName = "MyFragmentShader";               static_assert(std::char_traits<char>::length(defFragmentShaderName) < BufferSize);
	static constexpr auto defShaderProgramName  = "MyShaderProgram";                static_assert(std::char_traits<char>::length(defShaderProgramName)  < BufferSize);
	static constexpr auto defMaterialName       = "MyMaterial";                     static_assert(std::char_traits<char>::length(defMaterialName)       < BufferSize);
	static constexpr auto defFrameBufferName    = "MyFrameBuffer";                  static_assert(std::char_traits<char>::length(defFrameBufferName)    < BufferSize);
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
	case Sampler:
		std::strcpy(buf.data(), defSamplerName);
		titleText = std::format(TITLE_TEXT, "Sampler");
		confirmText = std::format(CONFIRM_TEXT, "a Sampler");
		break;
	case Texture:
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
	case Material:
		std::strcpy(buf.data(), defMaterialName);
		titleText = std::format(TITLE_TEXT, "Material");
		confirmText = std::format(CONFIRM_TEXT, "a Material");
		break;
	case FrameBuffer:
		std::strcpy(buf.data(), defFrameBufferName);
		titleText = std::format(TITLE_TEXT, "Frame Buffer");
		confirmText = std::format(CONFIRM_TEXT, "a Frame Buffer");
		break;
	case Model:
		break;
	default:
		std::unreachable(); // invalid enum
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
	ImGui::TextUnformatted(("Name: " + newAssetData.name).c_str());
	ImGui::TextUnformatted(("Path: " + newAssetData.path).c_str());
	ImGui::NewLine();
	ImGui::TextUnformatted(confirmText.c_str());
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
	case Sampler:
		CreateSamplerAsset();
		break;
	case Texture:
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
	case Material:
		CreateMaterialAsset();
		break;
	case FrameBuffer:
		CreateFrameBufferAsset();
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
	GUI& gui = this->gui.get();
	const auto sourceCode = CodeGenerator::GenerateComponentDeclaration(newAssetData.name);
	auto handle = gui.CORE->GetAssets()->CreateAssetAt<Component>(*newAssetData.currentFolder, newAssetData.name + Assets::ComponentDefinitionExtension, sourceCode);
	DOA_LOG_INFO("Succesfully created a new component asset named %s at %s", newAssetData.name.c_str(), newAssetData.path.c_str());
	gui.Events.OnAssetCreated(handle);
}
void NewAssetModal::CreateSamplerAsset() {
	GUI& gui = this->gui.get();
	Sampler temporary{ .Name = newAssetData.name };
	const auto data = temporary.Serialize();
	auto handle = gui.CORE->GetAssets()->CreateAssetAt<Sampler>(*newAssetData.currentFolder, newAssetData.name + Assets::SamplerExtension, data);
	DOA_LOG_INFO("Succesfully created a sampler asset named %s at %s", newAssetData.name.c_str(), newAssetData.path.c_str());
	gui.Events.OnAssetCreated(handle);
}
void NewAssetModal::CreateVertexShaderAsset() {
	GUI& gui = this->gui.get();
	const auto sourceCode = CodeGenerator::GenerateVertexShaderCode();
	auto handle = gui.CORE->GetAssets()->CreateAssetAt<Shader>(*newAssetData.currentFolder, newAssetData.name + Assets::VertexShaderExtension, sourceCode);
	DOA_LOG_INFO("Succesfully created a new vertex shader asset named %s at %s", newAssetData.name.c_str(), newAssetData.path.c_str());
	gui.Events.OnAssetCreated(handle);
}
void NewAssetModal::CreateTessellationControlAsset() {
	GUI& gui = this->gui.get();
	const auto sourceCode = CodeGenerator::GenerateTessellationControlShaderCode();
	auto handle = gui.CORE->GetAssets()->CreateAssetAt<Shader>(*newAssetData.currentFolder, newAssetData.name + Assets::TessellationControlShaderExtension, sourceCode);
	DOA_LOG_INFO("Succesfully created a new tessellation control shader asset named %s at %s", newAssetData.name.c_str(), newAssetData.path.c_str());
	gui.Events.OnAssetCreated(handle);
}
void NewAssetModal::CreateTessellationEvaluationAsset() {
	GUI& gui = this->gui.get();
	const auto sourceCode = CodeGenerator::GenerateTessellationEvaluationShaderCode();
	auto handle = gui.CORE->GetAssets()->CreateAssetAt<Shader>(*newAssetData.currentFolder, newAssetData.name + Assets::TessellationEvaluationShaderExtension, sourceCode);
	DOA_LOG_INFO("Succesfully created a new tessellation evaluation shader asset named %s at %s", newAssetData.name.c_str(), newAssetData.path.c_str());
	gui.Events.OnAssetCreated(handle);
}
void NewAssetModal::CreateGeometryShaderAsset() {
	GUI& gui = this->gui.get();
	const auto sourceCode = CodeGenerator::GenerateGeometryShaderCode();
	auto handle = gui.CORE->GetAssets()->CreateAssetAt<Shader>(*newAssetData.currentFolder, newAssetData.name + Assets::GeometryShaderExtension, sourceCode);
	DOA_LOG_INFO("Succesfully created a new geometry shader asset named %s at %s", newAssetData.name.c_str(), newAssetData.path.c_str());
	gui.Events.OnAssetCreated(handle);
}
void NewAssetModal::CreateFragmentShaderAsset() {
	GUI& gui = this->gui.get();
	const auto sourceCode = CodeGenerator::GenerateFragmentShaderCode();
	auto handle = gui.CORE->GetAssets()->CreateAssetAt<Shader>(*newAssetData.currentFolder, newAssetData.name + Assets::FragmentShaderExtension, sourceCode);
	DOA_LOG_INFO("Succesfully created a new fragment shader asset named %s at %s", newAssetData.name.c_str(), newAssetData.path.c_str());
	gui.Events.OnAssetCreated(handle);
}
void NewAssetModal::CreateShaderProgramAsset() {
	GUI& gui = this->gui.get();
	ShaderProgram temporary{ .Name = newAssetData.name };
	const auto data = temporary.Serialize();
	auto handle = gui.CORE->GetAssets()->CreateAssetAt<ShaderProgram>(*newAssetData.currentFolder, newAssetData.name + Assets::ShaderProgramExtension, data);
	DOA_LOG_INFO("Succesfully created a new shader program asset named %s at %s", newAssetData.name.c_str(), newAssetData.path.c_str());
	gui.Events.OnAssetCreated(handle);
}
void NewAssetModal::CreateMaterialAsset() {
	GUI& gui = this->gui.get();
	Material temporary{ newAssetData.name };
	const auto data = temporary.Serialize();
	auto handle = gui.CORE->GetAssets()->CreateAssetAt<Material>(*newAssetData.currentFolder, newAssetData.name + Assets::MaterialExtension, data);
	DOA_LOG_INFO("Succesfully created a material asset named %s at %s", newAssetData.name.c_str(), newAssetData.path.c_str());
	gui.Events.OnAssetCreated(handle);
}
void NewAssetModal::CreateFrameBufferAsset() {
	GUI& gui = this->gui.get();
	FrameBuffer temporary{ .Name = newAssetData.name };
	const auto data = temporary.Serialize();
	auto handle = gui.CORE->GetAssets()->CreateAssetAt<FrameBuffer>(*newAssetData.currentFolder, newAssetData.name + Assets::FrameBufferExtension, data);
	DOA_LOG_INFO("Succesfully created a frame buffer asset named %s at %s", newAssetData.name.c_str(), newAssetData.path.c_str());
	gui.Events.OnAssetCreated(handle);
}