#include "Editor.hpp"

#include "NeoDoa.hpp"

#include <iostream>
#include <sstream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "ComponentViews.hpp"

#include <IconsFontAwesome5.h>
#include <tinyfiledialogs.h>

#include "Tag.hpp"
#include "Transform.hpp"

static void DrawConsole() {
	ImGui::Begin("Console", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::SetWindowFontScale(0.9);

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);

	ImGuiIO& io = ImGui::GetIO();
	ImFont* font = io.Fonts->Fonts[1];
	float lineHeight = font->FontSize * 0.9 + ImGui::GetStyle().FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 6.0f, lineHeight };
	static LogSeverity severity = LogSeverity::TRACE;

	ImGui::PushFont(font);
	ImGui::PushStyleColor(ImGuiCol_Text, { 0.7, 0.7, 0.7, 1 });
	if (severity == LogSeverity::TRACE) {
		ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
		ImGui::Button(ICON_FA_INFO "###TRACE", buttonSize);
		ImGui::PopStyleColor(2);
	} else if (ImGui::Button(ICON_FA_INFO "###TRACE", buttonSize)) {
		severity = LogSeverity::TRACE;
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Show all messages");
	}
	ImGui::PopStyleColor();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Text, { 0.7, 1, 0.7, 1 });
	if (severity == LogSeverity::INFO) {
		ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
		ImGui::Button(ICON_FA_INFO "###INFO", buttonSize);
		ImGui::PopStyleColor(2);
	} else if (ImGui::Button(ICON_FA_INFO "###INFO", buttonSize)) {
		severity = LogSeverity::INFO;
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Show only messages with severity INFO or higher");
	}
	ImGui::PopStyleColor();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Text, { 1, 0.5, 0.1, 1 });
	if (severity == LogSeverity::WARNING) {
		ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
		ImGui::Button(ICON_FA_EXCLAMATION_CIRCLE "###WARNING", buttonSize);
		ImGui::PopStyleColor(2);
	} else if (ImGui::Button(ICON_FA_EXCLAMATION_CIRCLE "###WARNING", buttonSize)) {
		severity = LogSeverity::WARNING;
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Show only messages with severity WARNING or higher");
	}
	ImGui::PopStyleColor();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Text, { 1, 0.6, 0.6, 1 });
	if (severity == LogSeverity::ERRO) {
		ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
		ImGui::Button(" " ICON_FA_EXCLAMATION_TRIANGLE "###ERROR", buttonSize);
		ImGui::PopStyleColor(2);
	} else if (ImGui::Button(" " ICON_FA_EXCLAMATION_TRIANGLE "###ERROR", buttonSize)) {
		severity = LogSeverity::ERRO;
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Show only ERROR and FATAL messages");
	}
	ImGui::PopStyleColor();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Text, { 1, 0.4, 0.4, 1 });
	if (severity == LogSeverity::FATAL) {
		ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
		ImGui::Button(" " ICON_FA_EXCLAMATION_TRIANGLE "###FATAL", buttonSize);
		ImGui::PopStyleColor(2);
	} else if (ImGui::Button(" " ICON_FA_EXCLAMATION_TRIANGLE "###FATAL", buttonSize)) {
		severity = LogSeverity::FATAL;
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Show only FATAL messages");
	}
	ImGui::PopFont();
	ImGui::PopStyleColor();
	ImGui::SameLine();

	ImGui::Dummy({ ImGui::GetContentRegionAvail().x - 60, buttonSize.y });
	ImGui::SameLine();

	static int oldCount;
	if (ImGui::Button("Clear", { 60, buttonSize.y })) {
		Log::Clear();
		oldCount = 0;
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Clear the console");
	}

	ImGui::PopStyleVar(2);

	static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_ScrollY;
	ImGui::BeginTable("log", 2, flags, {ImGui::GetContentRegionAvail()});
	ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 30);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
	auto& messages = Log::_messages;
	for (auto& message : messages) {
		if (severity <= message._severity) {
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			const char* icon;
			ImVec4 color;

			float oldX = ImGui::GetCursorPosX();
			switch (message._severity) {
			case LogSeverity::TRACE:
				icon = ICON_FA_INFO;
				color = { 0.7, 0.7, 0.7, 1 };
				ImGui::SetCursorPosX(oldX + 14);
				break;
			case LogSeverity::INFO:
				icon = ICON_FA_INFO;
				color = { 0.7, 1, 0.7, 1 };
				ImGui::SetCursorPosX(oldX + 14);
				break;
			case LogSeverity::WARNING:
				icon = ICON_FA_EXCLAMATION_CIRCLE;
				color = { 1, 0.5, 0.1, 1 };
				ImGui::SetCursorPosX(oldX + 9);
				break;
			case LogSeverity::ERRO:
				icon = ICON_FA_EXCLAMATION_TRIANGLE;
				color = { 1, 0.6, 0.6, 1 };
				ImGui::SetCursorPosX(oldX + 8);
				break;
			case LogSeverity::FATAL:
				icon = ICON_FA_EXCLAMATION_TRIANGLE;
				color = { 1, 0.4, 0.4, 1 };
				ImGui::SetCursorPosX(oldX + 8);
				break;
			default:
				icon = "??";
				color = { 1, 0, 1, 1 };
				break;
			}

			ImGui::PushStyleColor(ImGuiCol_Text, color);

			ImGui::PushFont(font);
			ImGui::Text(icon);
			ImGui::SetCursorPosX(oldX);
			ImGui::PopFont();

			ImGui::TableSetColumnIndex(1);
			ImGui::PushFont(font);
			ImGui::Text(message._message.c_str());
			ImGui::PopFont();

			ImGui::PopStyleColor();

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
				ImGui::SetClipboardText(message._message.c_str());
			}
		}
	}
	ImGui::PopStyleVar();

	if (oldCount < messages.size()) {
		ImGui::SetScrollY(ImGui::GetScrollMaxY() + 100);
		oldCount = messages.size();
	}
	ImGui::EndTable();

	ImGui::SetWindowFontScale(1);
	ImGui::End();
}

static void DrawAssetManager(std::unique_ptr<Core>& core, std::weak_ptr<Scene> scene) {
	ImGui::Begin("Assets");

	ImGui::End();
}

static void DrawSceneSettings(std::unique_ptr<Core>& core, std::weak_ptr<Scene> scene, ImVec2 viewportSize) {
	auto ptr = scene.lock();

	ImGui::ShowDemoWindow();

	ImGui::Begin("Scene Stats/Settings");
	ImGui::Text("Draw Calls: %d", ptr->_renderer.drawCalls);
	ImGui::Text("Vertices: %d", ptr->_renderer.vertices);
	ImGui::Text("Indices: %d", ptr->_renderer.indices);
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::ColorEdit3("Clear Color", &core->ClearColor[0]);

	ImGui::Separator();
	static int camSelection = 1;
	ImGui::RadioButton("Ortho", &camSelection, 0);
	ImGui::RadioButton("Perspective", &camSelection, 1);
	ImGui::NewLine();
	ImGui::Text("Active Camera: %s", camSelection == 0 ? "Ortho" : "Perspective");
	ImGui::Text("Properties: ");
	if (camSelection == 0) {
		ImGui::DragFloat("Left", &ptr->_oc._left);
		ImGui::DragFloat("Right", &ptr->_oc._right);
		ImGui::DragFloat("Bottom", &ptr->_oc._bottom);
		ImGui::DragFloat("Top", &ptr->_oc._top);
		ImGui::DragFloat("Near", &ptr->_oc._near);
		ImGui::DragFloat("Far", &ptr->_oc._far);
	} else if (camSelection == 1) {
		static float fov = 90, near = 0.001, far = 1000;
		ImGui::DragFloat("FOV", &ptr->_pc._fov, 1, 1, 179);
		ImGui::DragFloat("Near", &ptr->_pc._near);
		ImGui::DragFloat("Far", &ptr->_pc._far);
		ptr->_pc._aspect = viewportSize.x / viewportSize.y;
	}
	ImGui::End();

	ImGui::Begin("Camera Controls");
	if (camSelection == 0) {
		FancyVectorWidget("Position", ptr->_oc.position, 0);
		glm::vec3 rot{ 0, 0, ptr->_oc.rotation };
		FancyVectorWidget("Rotation", rot, 0, Z);
		ptr->_oc.rotation = glm::radians(rot).z;
		glm::vec3 zoom{ 0, 0, ptr->_oc.zoom };
		FancyVectorWidget("Zoom", zoom, 1, Z);
		ptr->_oc.zoom = zoom.z;
	}
	else if (camSelection == 1) {
		FancyVectorWidget("Position", ptr->_pc.position, 0);
		FancyVectorWidget("Rotation", ptr->_pc.rotation, 0);
		glm::vec3 zoom{ 0, 0, ptr->_pc.zoom };
		FancyVectorWidget("Zoom", zoom, 1, Z);
		ptr->_pc.zoom = zoom.z;
	}
	ImGui::End();
}

static void DrawGizmos(std::unique_ptr<Core>& core, std::weak_ptr<Scene> scene, EntityID selectedEntity, ImGuizmo::OPERATION gizmoType) {
	if (selectedEntity != NULL_ENTT) {
		auto ptr = scene.lock();
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();

		float windowWidth = (float)ImGui::GetWindowWidth();
		float windowHeight = (float)ImGui::GetWindowHeight();
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

		glm::mat4 proj, view;
		if (ptr->_activeCamera == &ptr->_pc) {
			proj = ptr->_pc._projectionMatrix;
			view = ptr->_pc._viewMatrix;
		}
		else if (ptr->_activeCamera == &ptr->_oc) {
			proj = ptr->_oc._projectionMatrix;
			view = ptr->_oc._viewMatrix;
		}

		// Entity transform
		Transform& transform = ptr->Modules(selectedEntity)["Transform"].As<Transform>();
		glm::mat4 mm = CreateModelMatrixFromTransform(transform);

		bool snap = IsKeyPressed(KEY_LEFT_CONTROL);
		float snapValue = 0.5f;
		if (gizmoType == ImGuizmo::OPERATION::ROTATE) { snapValue = 5.0f; }
		float snapValues[3] = { snapValue, snapValue, snapValue };

		ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(proj), gizmoType, ImGuizmo::LOCAL, glm::value_ptr(mm), nullptr, snap ? snapValues : nullptr);

		if (ImGuizmo::IsUsing()) {
			{ // Decompose model matrix returned by ImGuizmo
				glm::mat4 LocalMatrix(mm);
				if (!glm::epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), glm::epsilon<float>())) {
					// perspective
					if (glm::epsilonNotEqual(LocalMatrix[0][3], static_cast<float>(0), glm::epsilon<float>()) ||
						glm::epsilonNotEqual(LocalMatrix[1][3], static_cast<float>(0), glm::epsilon<float>()) ||
						glm::epsilonNotEqual(LocalMatrix[2][3], static_cast<float>(0), glm::epsilon<float>())) {
						LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<float>(0);
						LocalMatrix[3][3] = static_cast<float>(1);
					}

					// translation
					transform.Translation() = glm::vec3(LocalMatrix[3]);
					LocalMatrix[3] = glm::vec4(0, 0, 0, LocalMatrix[3].w);

					glm::vec3 Row[3], Pdum3;

					// scale & shear.
					for (glm::length_t i = 0; i < 3; i++) {
						for (glm::length_t j = 0; j < 3; j++) {
							Row[i][j] = LocalMatrix[i][j];
						}
					}

					// Compute X scale factor and normalize first row.
					auto scale = transform.Scale();
					scale.x = length(Row[0]);
					Row[0] = glm::detail::scale(Row[0], static_cast<float>(1));
					scale.y = length(Row[1]);
					Row[1] = glm::detail::scale(Row[1], static_cast<float>(1));
					scale.z = length(Row[2]);
					Row[2] = glm::detail::scale(Row[2], static_cast<float>(1));

					glm::vec3 rot;
					rot.y = asin(-Row[0][2]);
					if (cos(rot.y) != 0) {
						rot.x = atan2(Row[1][2], Row[2][2]);
						rot.z = atan2(Row[0][1], Row[0][0]);
					} else {
						rot.x = atan2(-Row[2][0], Row[1][1]);
						rot.z = 0;
					}
					transform.Rotation() = rot;
				}
			}
		}
	}
}

static ImVec2 DrawViewport(std::unique_ptr<Core>& core, std::weak_ptr<Scene> scene, EntityID selectedEntity) {
	static bool drawGizmos = true;
	static auto gizmoType = ImGuizmo::OPERATION::TRANSLATE;

	ImGui::Begin("Viewport");

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);

	ImGuiIO& io = ImGui::GetIO();
	ImFont* font = io.Fonts->Fonts[1];
	ImGui::PushFont(font);
	if (drawGizmos) {
		ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
		if (ImGui::Button("Gizmos")) {
			drawGizmos = false;
		}
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	} else if (ImGui::Button("Gizmos")) {
		drawGizmos = true;
	}
	ImGui::SameLine();

	float lineHeight = font->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	if (gizmoType == ImGuizmo::OPERATION::TRANSLATE) {
		ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
		ImGui::Button(ICON_FA_ARROWS_ALT, buttonSize);
		ImGui::PopStyleColor(2);
	} else if (ImGui::Button(ICON_FA_ARROWS_ALT, buttonSize)) {
		gizmoType = ImGuizmo::OPERATION::TRANSLATE;
	}
	ImGui::SameLine();

	if (gizmoType == ImGuizmo::OPERATION::ROTATE) {
		ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
		ImGui::Button(ICON_FA_SYNC_ALT, buttonSize);
		ImGui::PopStyleColor(2);
	} else if (ImGui::Button(ICON_FA_SYNC_ALT, buttonSize)) {
		gizmoType = ImGuizmo::OPERATION::ROTATE;
	}
	ImGui::SameLine();

	if (gizmoType == ImGuizmo::OPERATION::SCALE) {
		ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
		ImGui::Button(ICON_FA_EXPAND_ALT, buttonSize);
		ImGui::PopStyleColor(2);
	} else if (ImGui::Button(ICON_FA_EXPAND_ALT, buttonSize)) {
		gizmoType = ImGuizmo::OPERATION::SCALE;
	}
	ImGui::SameLine();

	ImGui::Dummy({ ImGui::GetContentRegionAvail().x - buttonSize.x, buttonSize.y });
	ImGui::SameLine();

	if (core->_playing) {
		if (ImGui::Button(ICON_FA_STOP, buttonSize)) {
			core->_playing = false;
		}
	} else if (ImGui::Button(ICON_FA_PLAY, buttonSize)) {
		core->_playing = true;
	}

	ImGui::PopFont();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	ImGui::Image((void*)core->_offscreenBuffer->_tex, { viewportPanelSize.x, viewportPanelSize.y }, { 0, 1 }, { 1, 0 });
	if (drawGizmos) {
		DrawGizmos(core, scene, selectedEntity, gizmoType);
	}
	ImGui::End();

	return viewportPanelSize;
}

static void DrawGameObjectNode(std::unique_ptr<Core>& core, std::weak_ptr<Scene> scene, EntityID entity, EntityID* selectedEntity) {
	static ImGuiTreeNodeFlags flags;

	auto ptr = scene.lock();
	ScriptComponent& scr = ptr->Modules(entity);
	std::string label = scr["Tag"].As<Tag>().Label();
	Transform& transform = scr["Transform"].As<Transform>();
	auto* children = (CScriptArray*)transform._module->GetAddressOfProperty(5); //TODO add children and parent to Transform class
	std::string title;
	title.reserve(64);
	title.append(ICON_FA_CUBE " ");
	title.append(label);
	title.append("###GameObject");
	title.append(std::to_string(static_cast<std::uint32_t>(entity)));
	bool isLeaf = children->GetSize() == 0;
	if (!isLeaf) {
		flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	} else {
		flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}
	if (entity == *selectedEntity) {
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	bool opened = ImGui::TreeNodeEx(title.c_str(), flags);
	if (ImGui::BeginDragDropSource()) {
		ImGui::SetDragDropPayload("SELECTED_ENTT", &entity, sizeof(EntityID));
		std::string txt;
		txt.append("DragDrop - ");
		txt.append(label);
		ImGui::Text(txt.c_str());
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget()) {
		auto* payload = ImGui::AcceptDragDropPayload("SELECTED_ENTT");
		if (payload != nullptr) {
			EntityID dropped = *((EntityID*)payload->Data);
			std::vector<int> allParents;
			auto* parentTransform = &transform;
			while (parentTransform != nullptr) {
				allParents.emplace_back(static_cast<int>(parentTransform->GetID()));
				//parentTransform = *(asIScriptObject**)t->GetAddressOfProperty(5); //TODO add children and parent to Transform class
			}
			if (std::find(allParents.begin(), allParents.end(), (int)dropped) == allParents.end()) {
				Transform& droppedTransform = ptr->Modules(dropped)["Transform"].As<Transform>();
				transform.Adopt(droppedTransform);
				ImGui::EndDragDropTarget();
			}
		}
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_None) && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
		*selectedEntity = entity;
	}
	if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonRight)) {
		if (ImGui::BeginMenu("Attach Module")) {
			for (auto& pair : core->_angel->_modules) {
				const char* name = pair.first.c_str();
				if (ImGui::MenuItem(name)) {
					scr.Attach(name);
				}
			}
			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}
	if (opened) {
		for (int i = 0; i < children->GetSize(); i++) {
			auto** obj = (asIScriptObject**)children->At(i);
			int entityID = *(int*)(*obj)->GetAddressOfProperty(0);
			DrawGameObjectNode(core, scene, (EntityID)entityID, selectedEntity);
		}
		if (!isLeaf) {
			ImGui::TreePop();
		}
	}
}

static EntityID DrawSceneHierarchy(std::unique_ptr<Core>& core, std::weak_ptr<Scene> scene) {
	static EntityID selectedEntity{ NULL_ENTT };

	auto ptr = scene.lock();
	ImGui::Begin("Scene");
	if (ImGui::BeginDragDropTarget()) {
		auto* payload = ImGui::AcceptDragDropPayload("SELECTED_ENTT");
		if (payload != nullptr) {
			EntityID dropped = *((EntityID*)payload->Data);
			Transform& transform = ptr->Modules(dropped)["Transform"].As<Transform>();
			asIScriptObject*& parent = transform.GetAt<asIScriptObject*>(4);
			//transform.Parent().Disown(transform); //TODO add children and parent to Transform class
			ImGui::EndDragDropTarget();
		}
	}
	std::string title(ICON_FA_PROJECT_DIAGRAM " ");
	title.reserve(64);
	title.append(ptr->_name);
	if (ImGui::CollapsingHeader(title.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
		if (ImGui::BeginDragDropTarget()) {
			auto* payload = ImGui::AcceptDragDropPayload("SELECTED_ENTT");
			if (payload != nullptr) {
				EntityID dropped = *((EntityID*)payload->Data);
				Transform& transform = ptr->Modules(dropped)["Transform"].As<Transform>();
				asIScriptObject*& parent = transform.GetAt<asIScriptObject*>(4);
				//transform.Parent().Disown(transform); //TODO add children and parent to Transform class
				ImGui::EndDragDropTarget();
			}
		}
		ptr->_registry.view<ScriptComponent>().each([&core, &scene](EntityID e, ScriptComponent& scr) {
			asIScriptObject*& parent = scr["Transform"].GetAt<asIScriptObject*>(4); //TODO add children and parent to Transform class
			//if (scr["Transform"].As<Transform>().Parent() == nullptr) {
			if (parent == nullptr) {
				DrawGameObjectNode(core, scene, e, &selectedEntity);
			}
		});
	}
	if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
		selectedEntity = NULL_ENTT;
	}
	if (ImGui::BeginPopupContextWindow(0, ImGuiMouseButton_Right, false)) {
		if (ImGui::MenuItem("Create New Entity")) {
			ptr->CreateEntity();
		}
		ImGui::EndPopup();
	}
	ImGui::End();

	return selectedEntity;
}

static void DrawObjectModules(std::unique_ptr<Core>& core, std::weak_ptr<Scene> scene, EntityID selectedEntity) {
	auto ptr = scene.lock();
	ScriptComponent& scr = ptr->Modules(selectedEntity);
	for (Module& module : scr._modules) {
		asITypeInfo* type = module._module->GetObjectType();
		if (core->_angel->IsDefModule(type)) {
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		}
		std::string title = module._name;
		title.insert(0, ICON_FA_FILE_CODE " ");
		if (ImGui::CollapsingHeader(title.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
			if (core->_angel->IsDefModule(type)) {
				ImGui::PopFont();
			}
			if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonRight)) {
				if (ImGui::MenuItem("Detach")) {
					scr.Detach(title.c_str());
					ImGui::EndPopup();
					break; // iterator is invalidated!
				}
				ImGui::EndPopup();
			}
			std::vector<PropertyData>& props = core->_angel->_modules[type->GetName()];
			for (int i = 1; i < props.size(); i++) { // starting from 1, the property at 0 is always the "entity" and it's never shown on the editor
				PropertyData& prop = props[i];
				if (prop.isPrivate || prop.isProtected) { continue; } // don't show private or protected variables in editor!
				auto it = core->_angel->_enums.find(prop.typeName);
				if (it != core->_angel->_enums.end()) {
					EnumWidget(prop.prettyName.c_str(), module.GetAt<int>(i), it->second);
				} else if (prop.typeName == "int") {
					IntWidget(prop.prettyName.c_str(), module.GetAt<int>(i));
				} else if (prop.typeName == "float") {
					FloatWidget(prop.prettyName.c_str(), module.GetAt<float>(i));
				} else if (prop.typeName == "double") {
					DoubleWidget(prop.prettyName.c_str(), module.GetAt<double>(i));
				} else if (prop.typeName == "string") {
					StringWidget(prop.prettyName.c_str(), module.GetAt<std::string>(i));
				} else if (prop.typeName == "vec2") {
					FancyVectorWidget(prop.prettyName.c_str(), module.GetAt<glm::vec2>(i));
				} else if (prop.typeName == "vec3") {
					FancyVectorWidget(prop.prettyName.c_str(), module.GetAt<glm::vec3>(i), prop.name == "Scale" ? 1 : 0);
				} else if (prop.typeName == "vec4") {
					FancyVectorWidget(prop.prettyName.c_str(), module.GetAt<glm::vec4>(i));
				} else if (prop.typeName == "quat") {
					glm::quat& quat = module.GetAt<glm::quat>(i);
					glm::vec3 eulersDeg(glm::degrees(glm::eulerAngles(quat)));
					glm::vec3 old(eulersDeg);
					FancyVectorWidget(prop.prettyName.c_str(), eulersDeg, 0);
					quat = quat * glm::quat(glm::radians(eulersDeg - old));
				} else if (prop.typeName == "Model") {
					std::string title(ICON_FA_INDUSTRY " ");
					title.reserve(64);
					title.append(prop.prettyName);
					UneditableStringWidget(title.c_str(), module.GetAt<Model*>(i)->_name);
				} else if (prop.typeName == "Shader") {
					std::string title(ICON_FA_FILE_ALT " ");
					title.reserve(64);
					title.append(prop.prettyName);
					UneditableStringWidget(title.c_str(), module.GetAt<Shader*>(i)->_name);
				} else {
					std::string text;
					text.reserve(32);
					if (!prop.isReference) {
						asIScriptObject*& o = module.GetAt<asIScriptObject*>(i);
						if (o == nullptr) {
							text.append("nullptr (");
							text.append(prop.typeName);
						} else {
							text.append(prop.typeName);
							text.append(" (");
							Module m("tm", o);
							auto& mModules = ptr->Modules(m.GetID()); //GetID retrrns garbage :(
							text.append(mModules["Tag"].As<Tag>().Label());
						}
						text.append(")");
						UneditableStringWidget(prop.prettyName.c_str(), text);
						if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonRight)) {
							if (ImGui::MenuItem("Reset")) {
								//o = nullptr;
							}
							ImGui::EndPopup();
						}
						if (ImGui::BeginDragDropTarget()) {
							auto* payload = ImGui::AcceptDragDropPayload("SELECTED_ENTT");
							if (payload != nullptr) {
								EntityID e = *(EntityID*)payload->Data;
								auto& droppedModules = ptr->Modules(e);
								for (auto& module : droppedModules._modules) {
									if (module._name == prop.typeName) {
										//o = module;
									}
								}
							}
							ImGui::EndDragDropTarget();
						}
					} else {
						std::string name = prop.prettyName;
						name.append(" (Read-Only " ICON_FA_LOCK " )");
						Module m("tmp", &module.GetAt<asIScriptObject>(i));
						int id = static_cast<int>(m.GetID());
						if (id == -1) {
							text.append("unassigned (");
							text.append(prop.typeName);
						} else {
							text.append(prop.typeName);
							text.append(" (");
							text.append(ptr->Modules(m.GetID())["Tag"].As<Tag>().Label());
						}
						text.append(")");
						UneditableStringWidget(name.c_str(), text);
						if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonRight)) {
							if (ImGui::MenuItem("Reset")) {
								m.SetID(-1);
							}
							ImGui::EndPopup();
						}
						if (ImGui::BeginDragDropTarget()) {
							auto* payload = ImGui::AcceptDragDropPayload("SELECTED_ENTT");
							if (payload != nullptr) {
								EntityID e = *(EntityID*)payload->Data;
								auto& droppedModules = ptr->Modules(e);
								for (auto& module : droppedModules._modules) {
									if (module._name == prop.typeName) {
										m.SetID(module.GetID());
									}
								}
							}
							ImGui::EndDragDropTarget();
						}
					}
					if (ImGui::IsItemClicked()) {
						// focus...
					}
				}
			}
		} else {
			if (core->_angel->IsDefModule(type)) {
				ImGui::PopFont();
			}
		}
	}
}

static void DrawObserverPanel(std::unique_ptr<Core>& core, std::weak_ptr<Scene> scene, EntityID selectedEntity) {
	auto ptr = scene.lock();

	std::ostringstream title;
	if (selectedEntity != NULL_ENTT) {
		ScriptComponent& scr = ptr->Modules(selectedEntity);
		title << scr["Tag"].As<Tag>().Label();
		title << " - ";
	}
	title << "Observer";
	title << "###Observer";
	ImGui::Begin(title.str().c_str());
	if (selectedEntity != NULL_ENTT) {
		DrawObjectModules(core, scene, selectedEntity);
	} else {
		const char* text = "Select an object from the scene :)";
		ImVec2 size = ImGui::GetContentRegionAvail();
		ImVec2 txtSize = ImGui::CalcTextSize(text);
		ImGui::PushStyleColor(ImGuiCol_Text, { core->ClearColor.x, core->ClearColor.y, core->ClearColor.z, 1 });
		for (int i = 0; i < 5; i++) {
			ImGui::SameLine((size.x - txtSize.x) / 2, 0);
			ImGui::Text(text);
			ImGui::NewLine();
		}
		ImGui::PopStyleColor();
	}
	ImGui::End();
}

void LicenceNotice(const char* name, const char* licence) {
	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
	ImGui::TextColored({ 0.7, 0.7, 0.7, 1 }, name);
	ImGui::PopFont();
	std::stringstream ss;
	ss << "Licence###";
	ss << name;
	if (ImGui::CollapsingHeader(ss.str().c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick)) {
		ImGui::Text(licence);
	}
}

Editor::Editor() noexcept {
	neodoaBanner = CreateTexture("!!neodoa!!", "Images/social.png").value();
}

void Editor::operator() (void) {
	static std::unique_ptr<Core>& core = GetCore();
	std::weak_ptr<Scene> scene = FindActiveScene();

	static bool dockspaceOpen = true;
	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen) {
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
		window_flags |= ImGuiWindowFlags_NoBackground;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
	ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen) {
		ImGui::PopStyleVar(2);
	}

	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	float minWinSizeX = style.WindowMinSize.x;
	style.WindowMinSize = { 370, 150 };
	style.WindowMenuButtonPosition = ImGuiDir_None;
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, { 0.0f, 0.0f }, dockspace_flags);
	}

	style.WindowMinSize.x = minWinSizeX;

	bool ab = false, ab_open = true, lib_open = true;
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New", "Ctrl+N")) {
				//NewScene();
			}
			if (ImGui::MenuItem("Open...", "Ctrl+O")) {
				//OpenScene();
			}
			if (ImGui::MenuItem("Save...", "Ctrl+S")) {
				//SaveScene();
			}
			if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
				//SaveAsScene();
			}
			if (ImGui::MenuItem("Exit")){
				if (tinyfd_messageBox("Warning", "You may have unsaved changes. Are you sure you want to quit?", "yesno", "warning", 0)) {
					core->Stop();
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help")) {
			if (ImGui::MenuItem("About")) {
				ab = true;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	if (ab) {
		ImGui::OpenPopup("About");
	}
	auto center = ImGui::GetMainViewport()->GetCenter();
	static ImVec2 size{ 500, 400 };
	ImGui::SetNextWindowSize(size);
	ImGui::SetNextWindowPos({ center.x - size.x / 2, center.y - size.y / 2 });
	if (ImGui::BeginPopupModal("About", &ab_open, ImGuiWindowFlags_NoResize)) {
		auto bold = ImGui::GetIO().Fonts->Fonts[1];
		ImGui::PushFont(bold);
		ImGui::TextColored({ 0.7, 0.7, 0.7, 1 }, "NeoDoa ALPHA");
		ImGui::PopFont();
		ImGui::Image((void*)neodoaBanner.lock()->_glTextureID, { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().x / 2 }, { 0, 1 }, { 1, 0 });
		ImGui::Text("A game engine. Build: Windows");
		ImGui::Dummy({ 0, 20 });
		if (ImGui::Button("Libraries and Licences", { ImGui::GetContentRegionAvail().x, 30 })) {
			ImGui::OpenPopup("Libraries and Licences");
		}

		static ImVec2 libsSize{ 800, 600 };
		ImGui::SetNextWindowSize(libsSize);
		ImGui::SetNextWindowPos({ center.x - libsSize.x / 2, center.y - libsSize.y / 2 });
		if (ImGui::BeginPopupModal("Libraries and Licences", &lib_open, ImGuiWindowFlags_NoResize)) {
			LicenceNotice("NeoDoa", R"(	# NeoDoa Public Licence

		> Copyright(C)[2021][Doga Oruc]

		> NeoDoa Public Licence
		> TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

		1. Do whatever you like with the original work, just don't be a dick.

		   Being a dick includes - but is not limited to - the following instances:

		 1a. Outright copyright infringement - Don't just copy this and change the name.
		 1b. Selling the unmodified original with no work done what - so - ever, that's REALLY being a dick.
		 1c. Modifying the original work to contain hidden harmful content. That would make you a PROPER dick.

		2. If you become rich through modifications, related works / services, or supporting the original work,
		share the love. Only a dick would make loads off this work and not buy the original work's
		creator(s) a pint.

		3. Software is provided with no warranty. Asking for help won't make you a dick, but asking someone to
		write your code for you makes you a DONKEY dick. If you happen to solve your problem before any help arrives,
		you would submit the fix back to regain your status of non-dick.)");

			LicenceNotice("AngelScript", R"(	AngelCode Scripting Library
	Copyright © 2003 - 2020 Andreas Jönsson

	This software is provided 'as-is', without any express or implied warranty.In no event will the authors be
	held liable for any damages arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose, including commercial applications, and
	to alter itand redistribute it freely, subject to the following restrictions :

	The origin of this software must not be misrepresented; you must not claim that you wrote the original software.
	If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

	Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

	This notice may not be removed or altered from any source distribution.)");

			LicenceNotice("Assimp", R"(	Open Asset Import Library (assimp)

	Copyright (c) 2006-2016, assimp team
	All rights reserved.

	Redistribution and use of this software in source and binary forms,
	with or without modification, are permitted provided that the
	following conditions are met:

	* Redistributions of source code must retain the above
	  copyright notice, this list of conditions and the
	  following disclaimer.

	* Redistributions in binary form must reproduce the above
	  copyright notice, this list of conditions and the
	  following disclaimer in the documentation and/or other
	  materials provided with the distribution.

	* Neither the name of the assimp team, nor the names of its
	  contributors may be used to endorse or promote products
	  derived from this software without specific prior
	  written permission of the assimp team.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.



	******************************************************************************

	AN EXCEPTION applies to all files in the ./test/models-nonbsd folder.
	These are 3d models for testing purposes, from various free sources
	on the internet. They are - unless otherwise stated - copyright of
	their respective creators, which may impose additional requirements
	on the use of their work. For any of these models, see
	<model-name>.source.txt for more legal information. Contact us if you
	are a copyright holder and believe that we credited you inproperly or
	if you don't want your files to appear in the repository.


	******************************************************************************

	Poly2Tri Copyright (c) 2009-2010, Poly2Tri Contributors
	http://code.google.com/p/poly2tri/

	All rights reserved.
	Redistribution and use in source and binary forms, with or without modification,
	are permitted provided that the following conditions are met:

	* Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.
	* Redistributions in binary form must reproduce the above copyright notice,
	  this list of conditions and the following disclaimer in the documentation
	  and/or other materials provided with the distribution.
	* Neither the name of Poly2Tri nor the names of its contributors may be
	  used to endorse or promote products derived from this software without specific
	  prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
	PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
	LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
	NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.)");

			LicenceNotice("EnTT", R"(	The MIT License (MIT)

	Copyright (c) 2017-2020 Michele Caini

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copy of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copy or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.)");

			LicenceNotice("GLEW", R"(	The OpenGL Extension Wrangler Library
	Copyright (C) 2002-2007, Milan Ikits <milan ikits[]ieee org>
	Copyright (C) 2002-2007, Marcelo E. Magallon <mmagallo[]debian org>
	Copyright (C) 2002, Lev Povalahev
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	* Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.
	* Redistributions in binary form must reproduce the above copyright notice,
	  this list of conditions and the following disclaimer in the documentation
	  and/or other materials provided with the distribution.
	* The name of the author may be used to endorse or promote products
	  derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
	ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
	LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
	CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
	THE POSSIBILITY OF SUCH DAMAGE.


	Mesa 3-D graphics library
	Version:  7.0

	Copyright (C) 1999-2007  Brian Paul   All Rights Reserved.

	Permission is hereby granted, free of charge, to any person obtaining a
	copy of this software and associated documentation files (the "Software"),
	to deal in the Software without restriction, including without limitation
	the rights to use, copy, modify, merge, publish, distribute, sublicense,
	and/or sell copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included
	in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
	OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
	BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
	AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
	CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


	Copyright (c) 2007 The Khronos Group Inc.

	Permission is hereby granted, free of charge, to any person obtaining a
	copy of this software and/or associated documentation files (the
	"Materials"), to deal in the Materials without restriction, including
	without limitation the rights to use, copy, modify, merge, publish,
	distribute, sublicense, and/or sell copies of the Materials, and to
	permit persons to whom the Materials are furnished to do so, subject to
	the following conditions:

	The above copyright notice and this permission notice shall be included
	in all copies or substantial portions of the Materials.

	THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.)");

			LicenceNotice("GLFW", R"(	Copyright (c) 2002-2006 Marcus Geelnard

	Copyright (c) 2006-2019 Camilla Löwy

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would
	   be appreciated but is not required.

	2. Altered source versions must be plainly marked as such, and must not
	   be misrepresented as being the original software.

	3. This notice may not be removed or altered from any source
	   distribution.)");

			LicenceNotice("GLM", R"(	================================================================================
	OpenGL Mathematics (GLM)
	--------------------------------------------------------------------------------
	GLM is licensed under The Happy Bunny License or MIT License

	================================================================================
	The Happy Bunny License (Modified MIT License)
	--------------------------------------------------------------------------------
	Copyright (c) 2005 - G-Truc Creation

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	Restrictions:
	 By making use of the Software for military purposes, you choose to make a
	 Bunny unhappy.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.

	================================================================================
	The MIT License
	--------------------------------------------------------------------------------
	Copyright (c) 2005 - G-Truc Creation

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.)");

			LicenceNotice("IconFontCppHeaders", R"(	Copyright (c) 2017 Juliette Foucaut and Doug Binks

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.)");

			LicenceNotice("Dear ImGui", R"(	The MIT License (MIT)

	Copyright (c) 2014-2020 Omar Cornut

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.)");

			LicenceNotice("STBI", R"(	This software is available under 2 licenses -- choose whichever you prefer.
	------------------------------------------------------------------------------
	ALTERNATIVE A - MIT License
	Copyright (c) 2017 Sean Barrett
	Permission is hereby granted, free of charge, to any person obtaining a copy of
	this software and associated documentation files (the "Software"), to deal in
	the Software without restriction, including without limitation the rights to
	use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
	of the Software, and to permit persons to whom the Software is furnished to do
	so, subject to the following conditions:
	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
	------------------------------------------------------------------------------
	ALTERNATIVE B - Public Domain (www.unlicense.org)
	This is free and unencumbered software released into the public domain.
	Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
	software, either in source code form or as a compiled binary, for any purpose,
	commercial or non-commercial, and by any means.
	In jurisdictions that recognize copyright laws, the author or authors of this
	software dedicate any and all copyright interest in the software to the public
	domain. We make this dedication for the benefit of the public at large and to
	the detriment of our heirs and successors. We intend this dedication to be an
	overt act of relinquishment in perpetuity of all present and future rights to
	this software under copyright law.
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
	ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
	WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.)");

			LicenceNotice("TinyFileDialogs", R"(	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.
	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:
	1. The origin of this software must not be misrepresented; you must not
	claim that you wrote the original software.  If you use this software
	in a product, an acknowledgment in the product documentation would be
	appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.)");

			LicenceNotice("YAML-CPP", R"(	Copyright (c) 2008-2015 Jesse Beder.

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.)");

			ImGui::EndPopup();
		}
		ImGui::EndPopup();
	}

	EntityID selected = DrawSceneHierarchy(core, scene);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
	ImVec2 viewportPanelSize = DrawViewport(core, scene, selected);
	ImGui::PopStyleVar();

	DrawSceneSettings(core, scene, viewportPanelSize);
	DrawObserverPanel(core, scene, selected);

	DrawConsole();
	DrawAssetManager(core, scene);

	ImGui::End();
}
