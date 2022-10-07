#include "Editor.hpp"

#include "NeoDoa.hpp"

#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>



#include <IconsFontAwesome5Pro.h>
#include <tinyfiledialogs.h>

#include <imgui_internal.h>
/*
static Entity selectedEntity{ NULL_ENTT }; // obvious
static Entity highlightedEntity{ NULL_ENTT }; // set when user clicks a reference field of a script
static std::unordered_map<std::string, std::string> moduleIcons {
    { "Tag", ICON_FA_TAG " "},
    { "Transform", ICON_FA_MAP_MARKER_ALT " "},
    { "Camera", ICON_FA_VIDEO " "},
    { "ModelRenderer", ICON_FA_COCKTAIL " "},
};

// New/Open/Save
static void NewProject(Editor* editor);
static void OpenProject(std::unique_ptr<Core>& core, Editor* editor);
static void NewScene(FNode* parent, std::string_view name);
static void OpenScene(Editor* editor, FNode* sceneFile);
static void SaveScene(Editor* editor, std::weak_ptr<Scene> scene);

// Scene Hierarchy
static Entity DrawSceneHierarchy(float delta, std::unique_ptr<Core>& core, std::weak_ptr<Scene> scene);
static void DrawGameObjectNode(float delta, std::unique_ptr<Core>& core, std::weak_ptr<Scene> scene, Entity entity);

// Viewport
static ImVec2 DrawViewport(std::unique_ptr<Core>& core, std::weak_ptr<Scene> scene, float deltaTime);
static void DrawGizmos(std::unique_ptr<Core>& core, std::weak_ptr<Scene> scene, ImVec2 viewportPosition, ImVec2 viewportSize, ImGuizmo::OPERATION gizmoType, ImGuizmo::MODE gizmoMode);
static void DrawCubeControl(std::unique_ptr<Core>& core, std::weak_ptr<Scene> scene, ImVec2 viewportPosition, ImVec2 viewportSize, float& yaw, float& pitch);

// Scene Settings
static void DrawSceneSettings(std::unique_ptr<Core>& core, std::weak_ptr<Scene> scene, ImVec2 viewportSize);

// Observer Panel
static void DrawObserverPanel(std::unique_ptr<Core>& core, std::weak_ptr<Scene> scene);
static void DrawObjectModules(std::unique_ptr<Core>& core, std::weak_ptr<Scene> scene);
static void DrawModulePropertyGUI(Module& module, PropertyData& property, int propertyIndex);

// Console
static void DrawConsole();

// Asset Manager
static void DrawAssetManager(Editor* editor, std::unique_ptr<Core>& core, std::weak_ptr<Scene> scene);
static void DrawDirectoryRecursive(Editor* editor, std::weak_ptr<Scene> scene, FNode* root);

// Licence Notice
static void LicenceNotice(const char* name, const char* licence);


// Impls
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
            const char* tooltipMessage;

            float oldX = ImGui::GetCursorPosX();
            switch (message._severity) {
            case LogSeverity::TRACE:
                icon = ICON_FA_INFO;
                color = { 0.7, 0.7, 0.7, 1 };
                tooltipMessage = "General purpose hints/clues";
                ImGui::SetCursorPosX(oldX + 14);
                break;
            case LogSeverity::INFO:
                icon = ICON_FA_INFO;
                color = { 0.7, 1, 0.7, 1 };
                tooltipMessage = "General notices";
                ImGui::SetCursorPosX(oldX + 14);
                break;
            case LogSeverity::WARNING:
                icon = ICON_FA_EXCLAMATION_CIRCLE;
                color = { 1, 0.5, 0.1, 1 };
                tooltipMessage = "Explanations to potentially unwanted situations";
                ImGui::SetCursorPosX(oldX + 9);
                break;
            case LogSeverity::ERRO:
                icon = ICON_FA_EXCLAMATION_TRIANGLE;
                color = { 1, 0.6, 0.6, 1 };
                tooltipMessage = "Unexpected behavior";
                ImGui::SetCursorPosX(oldX + 8);
                break;
            case LogSeverity::FATAL:
                icon = ICON_FA_EXCLAMATION_TRIANGLE;
                color = { 1, 0.4, 0.4, 1 };
                tooltipMessage = "Expected(!), but potentially very fragile situations. These will crash your game if you ship it with these :)";
                ImGui::SetCursorPosX(oldX + 8);
                break;
            case LogSeverity::OPENGL:
                icon = ICON_FA_SERVER;
                color = { 0.32, 0.51, 0.62, 1 };
                tooltipMessage = "OpenGL Server message";
                ImGui::SetCursorPosX(oldX + 9);
                break;
            case LogSeverity::VULKAN:
                icon = ICON_FA_SERVER;
                color = { 0.62, 0.11, 0.13, 1 };
                tooltipMessage = "Vulkan Server message";
                ImGui::SetCursorPosX(oldX + 9);
                break;
            case LogSeverity::DIRECTX:
                icon = ICON_FA_SERVER;
                color = { 0.48, 0.71, 0, 1 };
                tooltipMessage = "Direct-X Server message";
                ImGui::SetCursorPosX(oldX + 9);
                break;
            default:
                icon = "??";
                color = { 1, 0, 1, 1 };
                tooltipMessage = "this shouldn't be here";
                break;
            }

            ImGui::PushStyleColor(ImGuiCol_Text, color);

            ImGui::PushFont(font);
            ImGui::Text(icon);
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text(tooltipMessage);
                ImGui::EndTooltip();
            }
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

static void DrawDirectoryRecursive(Editor* editor, std::weak_ptr<Scene> scene, FNode* root) {
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    if (!root->_isDir) {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    std::string title = root->_name;
    std::string titleNoIcon = title;
    if (root->_ext == ".as" || root->_ext == ".sh") {
        title.insert(0, ICON_FA_CODE " ");
    } else if (root->_ext == ".fbx") {
        title.insert(0, ICON_FA_CUBE " ");
    } else if (root->_ext == ".scn") {
        title.insert(0, ICON_FA_PROJECT_DIAGRAM " ");
    } else if (root->_ext == ".doa") {
        title.insert(0, ICON_FA_ARCHIVE " ");
    } else if (root->_ext == ".ttf") {
        title.insert(0, ICON_FA_FONT " ");
    } else if (root->_ext == ".conf") {
        title.insert(0, ICON_FA_COG " ");
    } else if (root->_ext == ".mat") {
        title.insert(0, ICON_FA_CIRCLE " ");
    } else if (root->_ext == ".png" || root->_ext == ".jpg" || root->_ext == ".jpeg") {
        title.insert(0, ICON_FA_FILE_IMAGE " ");
    } else if (root->_isDir) {
        title.insert(0, ICON_FA_FOLDER " ");
    } else {
        title.insert(0, ICON_FA_FILE " ");
    }

    if (ImGui::TreeNodeEx(title.c_str(), flags)) {
        if (root->_isDir) {
            for (auto& child : root->_children) {
                DrawDirectoryRecursive(editor, scene, &child);
            }
        } else if (root->_isFile) {
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                if (root->_ext == ".scn") {
                    if (!scene.expired() && selectedEntity != NULL_ENTT) {
                        scene.lock()->Modules(selectedEntity)["Transform"].As<Transform>().Selected() = false;
                    }
                    selectedEntity = NULL_ENTT;
                    OpenScene(editor, root);
                    // change scene
                } else if (root->_ext == ".as" || root->_ext == ".sh" || root->_ext == ".mat") {
                    // open txt file
                } else if (root->_ext == ".png" || root->_ext == ".jpg" || root->_ext == ".jpeg") {
                    // preview image
                }
            }
        }
        if (ImGui::BeginPopupContextWindow(nullptr, ImGuiPopupFlags_MouseButtonRight)) {
            if (ImGui::MenuItem("Create New Scene")) {
                const char* name = nullptr;
                bool nameOK = false;
                while (!nameOK) {
                    name = tinyfd_inputBox("Enter a name for the new scene", "Enter a name for the new scene", "New Scene");
                    if (name) {
                        nameOK = std::string(name) != "";
                        if (!nameOK) {
                            tinyfd_messageBox("Warning", "Scenes cannot be unnamed.", "ok", "warning", 1);
                        }
                    } else {
                        break;
                    }
                }
                if(nameOK) {
                    NewScene(root, name);
                }
            }
            ImGui::EndPopup();
        }
        ImGui::TreePop();
    }
}

static void DrawAssetManager(Editor* editor, std::unique_ptr<Core>& core, std::weak_ptr<Scene> scene) {
    static FNode* assetRoot = nullptr;

    auto& proj = editor->_openProject;
    ImGui::Begin("Assets", 0, ImGuiWindowFlags_MenuBar);
    if (ImGui::BeginMenuBar()) {
        if (ImGui::MenuItem("Refresh")) {
            if (proj) {
                proj->_assets.ReScan();
            } else {
                DOA_LOG_WARNING("Didn't refresh! No open project.");
            }
        }
        ImGui::EndMenuBar();
    }
    if (proj) {
        assetRoot = &proj->_assets._root;
    }
    if (assetRoot) {
        DrawDirectoryRecursive(editor, scene, assetRoot);
    }
    ImGui::End();
}

static void DrawSceneSettings(std::unique_ptr<Core>& core, std::weak_ptr<Scene> scene, ImVec2 viewportSize) {
    auto ready = !scene.expired();
    auto ptr = scene.lock();

    ImGui::ShowDemoWindow();

    ImGui::Begin("Scene Stats/Settings");
    if(ready) {
        ImGui::Text("Draw Calls: %d", ptr->_renderer.drawCalls);
        ImGui::Text("Vertices: %d", ptr->_renderer.vertices);
        ImGui::Text("Indices: %d", ptr->_renderer.indices);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::ColorEdit3("Clear Color", &ptr->ClearColor[0]);
        ImGui::ColorEdit3("Selection Outline Color", &ptr->SelectionOutlineColor[0]);

        ImGui::Separator();
        static int camSelection = 1;
        ImGui::RadioButton("Ortho", &camSelection, 0);
        ImGui::RadioButton("Perspective", &camSelection, 1);
        ImGui::NewLine();
        ImGui::Text("Active Camera: %s", camSelection == 0 ? "Ortho" : "Perspective");
        ImGui::Text("Properties: ");
        if (camSelection == 0) {
            ptr->_activeCamera = &ptr->_oc;
            ImGui::DragFloat("Left", &ptr->_oc._left);
            ImGui::DragFloat("Right", &ptr->_oc._right);
            ImGui::DragFloat("Bottom", &ptr->_oc._bottom);
            ImGui::DragFloat("Top", &ptr->_oc._top);
            ImGui::DragFloat("Near", &ptr->_oc._near);
            ImGui::DragFloat("Far", &ptr->_oc._far);
        } else if (camSelection == 1) {
            ptr->_activeCamera = &ptr->_pc;
            ImGui::DragFloat("FOV", &ptr->_pc._fov, 1, 45, 135);
            ImGui::DragFloat("Near", &ptr->_pc._near);
            ImGui::DragFloat("Far", &ptr->_pc._far);
            ptr->_pc._aspect = viewportSize.x / viewportSize.y;
        }
    }
    ImGui::End();

    ImGui::Begin("Camera Controls");
    if (ready) {
        FancyVectorWidget("Eye", ptr->_activeCamera->eye, 0);
        FancyVectorWidget("Forward", ptr->_activeCamera->forward, 0);
        FancyVectorWidget("Up", ptr->_activeCamera->up, 0);
        FloatWidget("Zoom", ptr->_activeCamera->zoom);
    }
    ImGui::End();
}

static void DrawGizmos(std::unique_ptr<Core>& core, std::weak_ptr<Scene> scene, ImVec2 viewportPosition, ImVec2 viewportSize, ImGuizmo::OPERATION gizmoType, ImGuizmo::MODE gizmoMode) {
    auto ptr = scene.lock();
    ImGuizmo::SetDrawlist();
    if (ptr->_activeCamera == &ptr->_pc) {
        ImGuizmo::SetOrthographic(false);
    } else if (ptr->_activeCamera == &ptr->_oc) {
        ImGuizmo::SetOrthographic(true);
    }
    glm::mat4 proj = ptr->_activeCamera->_projectionMatrix;
    glm::mat4 view = ptr->_activeCamera->_viewMatrix;

    if (selectedEntity != NULL_ENTT) {
        ImGuizmo::SetRect(viewportPosition.x, viewportPosition.y, viewportSize.x, viewportSize.y);

        // Entity transform
        Transform& transform = ptr->Modules(selectedEntity)["Transform"].As<Transform>();
        glm::mat4 mm = CreateModelMatrixFromTransform(transform);

        bool snap = IsKeyPressed(KEY_LEFT_CONTROL);
        float snapValue = 0.5f;
        if (gizmoType == ImGuizmo::OPERATION::ROTATE) { snapValue = 5.0f; }
        float snapValues[3] = { snapValue, snapValue, snapValue };

        //glm::mat4 onlyTranslation = glm::mat4(1);
        //onlyTranslation = glm::translate(onlyTranslation, transform.Translation());
        //ImGuizmo::DrawGrid(glm::value_ptr(view), glm::value_ptr(proj), glm::value_ptr(onlyTranslation), 5);
        ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(proj), gizmoType, gizmoMode, glm::value_ptr(mm), nullptr, snap ? snapValues : nullptr);

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
                    auto& scale = transform.Scale();
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

static void DrawCubeControl(std::unique_ptr<Core>& core, std::weak_ptr<Scene> scene, ImVec2 viewportPosition, ImVec2 viewportSize, float& yaw, float& pitch) {
    auto ptr = scene.lock();
    glm::mat4 proj = ptr->_activeCamera->_projectionMatrix;
    glm::mat4 view = ptr->_activeCamera->_viewMatrix;
    ImGuizmo::ViewManipulate(glm::value_ptr(view), 8, { viewportPosition.x + viewportSize.x - 128 , viewportPosition.y }, { 128, 128 }, 0x10101080);
    ptr->_activeCamera->forward = glm::normalize(glm::vec3(-view[0].z, -view[1].z, -view[2].z)); // forward is INVERTED!!!

    // don't change up vector, fuck space sims. up being something other than 0, 1, 0 is VERBOTEN!
    //ptr->_activeCamera->up = glm::normalize(glm::vec3(view[0].y, view[1].y, view[2].y));

    yaw = glm::degrees(atan2(ptr->_activeCamera->forward.z, ptr->_activeCamera->forward.x));
    pitch = glm::degrees(asin(ptr->_activeCamera->forward.y));
}

static ImVec2 DrawViewport(std::unique_ptr<Core>& core, std::weak_ptr<Scene> scene, float deltaTime) {
    static bool drawGizmos = true;
    static auto gizmoMode = ImGuizmo::MODE::LOCAL;
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

    ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
    if (gizmoMode == ImGuizmo::MODE::WORLD) {
        if (ImGui::Button(ICON_FA_GLOBE " World")) {
            gizmoMode = ImGuizmo::MODE::LOCAL;
        }
    } else if (gizmoMode == ImGuizmo::MODE::LOCAL) {
        if (ImGui::Button(ICON_FA_MALE " Local")) {
            gizmoMode = ImGuizmo::MODE::WORLD;
        }
    }
    ImGui::PopStyleColor(2);
    ImGui::SameLine();

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

    bool ready = !scene.expired();
    if (ready) {
        if (core->_playing) {
            if (ImGui::Button(ICON_FA_STOP, buttonSize)) {
                core->_playing = false;
            }
        } else if (ImGui::Button(ICON_FA_PLAY, buttonSize)) {
            core->_playing = true;
        }
    }

    ImGui::PopFont();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    ImVec2 viewportPosition = { ImGui::GetWindowPos().x + ImGui::GetCursorPos().x, ImGui::GetWindowPos().y + ImGui::GetCursorPos().y };

    if (ready) {
        ImGui::Image((void*)core->_offscreenBuffer->_tex, { viewportSize.x, viewportSize.y }, { 0, 1 }, { 1, 0 });

        // Below is scene cam controls and gizmos
        static float yaw = -90, pitch = 0;
        if (drawGizmos) {
            DrawGizmos(core, scene, viewportPosition, viewportSize, gizmoType, gizmoMode);
        }
        DrawCubeControl(core, scene, viewportPosition, viewportSize, yaw, pitch);

        std::shared_ptr<Scene> ptr = scene.lock();
        glm::vec3& eye = ptr->_activeCamera->eye;
        glm::vec3& forward = ptr->_activeCamera->forward;
        glm::vec3& up = ptr->_activeCamera->up;
        float& zoom = ptr->_activeCamera->zoom;

        static ImVec2 oldDelta;
        static bool rightClick = false; // true when right clicked when mouse was on image, stays true until right click is released.
        if (ImGui::IsItemHovered()) {
            zoom += ImGui::GetIO().MouseWheel / 100;
            zoom = std::max(1.f, zoom);
            if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
                rightClick = true;
                glfwSetInputMode(core->_window->_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            } else {
                rightClick = false;
            }
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
                //select object
                //glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
            }
        }

        if (!ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
            rightClick = false;
            oldDelta = { 0, 0 };
            glfwSetInputMode(core->_window->_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        static float cameraSpeed = 30;
        if (rightClick) {
            ImVec2 v = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
            ImVec2 delta = { (v.x - oldDelta.x) / 10, (oldDelta.y - v.y) / 10 };
            if (ptr->_activeCamera == &ptr->_oc) {
                eye.x -= delta.x; // want the movement of the camera to be the inverse
                eye.y -= delta.y; // of mouse. like how you phone's touch screen works.
            } else if (ptr->_activeCamera == &ptr->_pc) {
                glm::vec3 right = glm::normalize(glm::cross(forward, up)) * (cameraSpeed * deltaTime);
                glm::vec3 fwd = glm::normalize(forward) * (cameraSpeed * deltaTime);
                if (IsKeyPressed(KEY_W)) {
                    eye += fwd;
                }
                if (IsKeyPressed(KEY_A)) {
                    eye -= right;
                }
                if (IsKeyPressed(KEY_S)) {
                    eye -= fwd;
                }
                if (IsKeyPressed(KEY_D)) {
                    eye += right;
                }
                yaw += delta.x;
                pitch += delta.y;
                if (pitch > 89.0f) {
                    pitch = 89.0f;
                } else if (pitch < -89.0f) {
                    pitch = -89.0f;
                }
                glm::vec3 direction;
                direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                direction.y = sin(glm::radians(pitch));
                direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                forward = glm::normalize(direction);
            }
            oldDelta = v;
        }
    }
    ImGui::End();

    return viewportSize;
}

static void DrawGameObjectNode(float delta, std::unique_ptr<Core>& core, std::weak_ptr<Scene> scene, Entity entity) {
    auto ptr = scene.lock();
    ScriptComponent& scr = ptr->Modules(entity);
    std::string label = scr["Tag"].As<Tag>().Label();
    Transform& transform = scr["Transform"].As<Transform>();
    auto& children = transform.Children(); //TODO add children and parent to Transform class
    std::string title;
    title.reserve(64);
    title.append(ICON_FA_CUBE " ");
    title.append(label);
    title.append("###GameObject");
    title.append(std::to_string(static_cast<std::uint32_t>(entity)));

    static ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;
    static float time = 0;
    static const float highlightExpire = 2;
    bool isLeaf = children.GetSize() == 0;
    if (!isLeaf) {
        flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    } else {
        flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }
    if (entity == selectedEntity) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }
    if (entity == highlightedEntity) {
        flags |= ImGuiTreeNodeFlags_Selected;
        static auto origHeaderColor = ImGui::GetStyleColorVec4(ImGuiCol_Header);
        static auto origTextColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
        float factor = time / highlightExpire;
        ImGui::PushStyleColor(ImGuiCol_Header, {
            eze::easeInSine(1.0f, origHeaderColor.x, factor),
            eze::easeInSine(1.0f, origHeaderColor.y, factor),
            eze::easeInSine(0.31f, origHeaderColor.z, factor),
            1
        });
        ImGui::PushStyleColor(ImGuiCol_Text, {
            eze::easeInSine(0.0f, origTextColor.x, factor),
            eze::easeInSine(0.0f, origTextColor.y, factor),
            eze::easeInSine(0.0f, origTextColor.z, factor),
            1
        });
        time += delta;

        static Entity lastHighlighted = highlightedEntity;
        // if highlighted entity changed during the highlight anim., restart the anim
        if (lastHighlighted != highlightedEntity) {
            time = 0;
        }
        lastHighlighted = highlightedEntity;
    }
    bool opened = ImGui::TreeNodeEx(title.c_str(), flags);
    if (entity == highlightedEntity) {
        ImGui::PopStyleColor(2);
        if (time > highlightExpire) {
            time = 0;
            highlightedEntity = NULL_ENTT;
        }
    }
    if (ImGui::BeginDragDropSource()) {
        ImGui::SetDragDropPayload("SELECTED_ENTT", &entity, sizeof(Entity));
        std::string txt;
        txt.append("DragDrop - ");
        txt.append(label);
        ImGui::Text(txt.c_str());
        ImGui::EndDragDropSource();
    }
    if (ImGui::BeginDragDropTarget()) {
        auto* payload = ImGui::AcceptDragDropPayload("SELECTED_ENTT");
        if (payload != nullptr) {
            Entity dropped = *((Entity*)payload->Data);
            std::vector<int> allParents;
            auto* parentTransform = &transform;
            while (parentTransform != nullptr) {
                allParents.emplace_back(static_cast<int>(parentTransform->GetID()));
                parentTransform = parentTransform->Parent();
            }
            if (std::find(allParents.begin(), allParents.end(), (int)dropped) == allParents.end()) {
                Transform& droppedTransform = ptr->Modules(dropped)["Transform"].As<Transform>();
                transform.Adopt(droppedTransform);
                ImGui::EndDragDropTarget();
            }
        }
    }
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_None) && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        if (selectedEntity != NULL_ENTT) {
            ptr->Modules(selectedEntity)["Transform"].As<Transform>().Selected() = false;
        }
        selectedEntity = entity;
        ptr->Modules(selectedEntity)["Transform"].As<Transform>().Selected() = true;
    }
    if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonRight)) {
        if (ImGui::BeginMenu(ICON_FA_LINK " Attach Module")) {
            for (auto& pair : core->_angel->_modules) {
                std::string name = pair.first;
                auto icon = moduleIcons.find(name);
                if (icon != moduleIcons.end()) {
                    name.insert(0, icon->second);
                } else {
                    name.insert(0, ICON_FA_FILE_CODE " ");
                }
                if (ImGui::MenuItem(name.c_str())) {
                    scr.Attach(pair.first);
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }
    if (opened) {
        for (int i = 0; i < children.GetSize(); i++) {
            auto** obj = (asIScriptObject**)children.At(i);
            Module tmp("tmp", *obj);
            DrawGameObjectNode(delta, core, scene, tmp.GetID());
        }
        if (!isLeaf) {
            ImGui::TreePop();
        }
    }
}

static Entity DrawSceneHierarchy(float delta, std::unique_ptr<Core>& core, std::weak_ptr<Scene> scene) {
    auto ready = !scene.expired();
    ImGui::Begin("Scene");
    if (ready) {
        auto ptr = scene.lock();
        if (ImGui::BeginDragDropTarget()) {
            auto* payload = ImGui::AcceptDragDropPayload("SELECTED_ENTT");
            if (payload != nullptr) {
                Entity dropped = *((Entity*)payload->Data);
                TransformComponent& tr = ptr->GetComponent<TransformComponent>(dropped);
                tr.SetParent(nullptr);
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
                    Entity dropped = *((Entity*)payload->Data);
                    TransformComponent& tr = ptr->GetComponent<TransformComponent>(dropped);
                    tr.SetParent(nullptr);
                    ImGui::EndDragDropTarget();
                }
            }
            ptr->_registry.view<ScriptComponent>().each([&delta, &core, &scene](Entity e, ScriptComponent& scr) {
                if (scr["Transform"].As<Transform>().Parent() == nullptr) {
                    DrawGameObjectNode(delta, core, scene, e);
                }
            });
        }
        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
            if (selectedEntity != NULL_ENTT) {
                ptr->Modules(selectedEntity)["Transform"].As<Transform>().Selected() = false;
            }
            selectedEntity = NULL_ENTT;
        }
        if (ImGui::BeginPopupContextWindow(0, ImGuiMouseButton_Right, false)) {
            if (ImGui::MenuItem(ICON_FA_PLUS " Create New Entity")) {
                ptr->CreateEntity();
            }
            ImGui::Separator();
            if (ImGui::MenuItem(ICON_FA_WINDOW_CLOSE " Close Scene")) {
                ChangeScene({});
            }
            ImGui::EndPopup();
        }
    }
    ImGui::End();

    return selectedEntity;
}

static void DrawModulePropertyGUI(Module& module, PropertyData& property, int propertyIndex) {
    static auto& core = GetCore();
    auto ptr = FindActiveScene().lock();
    auto it = core->_angel->_enums.find(property.typeName);
    if (it != core->_angel->_enums.end()) {
        EnumWidget(property.prettyName.c_str(), module.GetAt<int>(propertyIndex), it->second);
    } else if (property.typeName == "int") {
        IntWidget(property.prettyName.c_str(), module.GetAt<int>(propertyIndex));
    } else if (property.typeName == "float") {
        FloatWidget(property.prettyName.c_str(), module.GetAt<float>(propertyIndex));
    } else if (property.typeName == "double") {
        DoubleWidget(property.prettyName.c_str(), module.GetAt<double>(propertyIndex));
    } else if (property.typeName == "bool") {
        BoolWidget(property.prettyName.c_str(), module.GetAt<bool>(propertyIndex));
    } else if (property.typeName == "string") {
        std::string title(ICON_FA_TEXT " ");
        title.reserve(64);
        title.append(property.prettyName);
        StringWidget(title.c_str(), module.GetAt<std::string>(propertyIndex));
    } else if (property.typeName == "vec2") {
        std::string title(ICON_FA_EXTERNAL_LINK_SQUARE_ALT " ");
        title.reserve(64);
        title.append(property.prettyName);
        FancyVectorWidget(title.c_str(), module.GetAt<glm::vec2>(propertyIndex));
    } else if (property.typeName == "vec3") {
        std::string title(ICON_FA_EXTERNAL_LINK_SQUARE_ALT " ");
        title.reserve(64);
        title.append(property.prettyName);
        FancyVectorWidget(title.c_str(), module.GetAt<glm::vec3>(propertyIndex), property.name == "Scale" ? 1 : 0);
    } else if (property.typeName == "vec4") {
        std::string title(ICON_FA_EXTERNAL_LINK_SQUARE_ALT " ");
        title.reserve(64);
        title.append(property.prettyName);
        FancyVectorWidget(title.c_str(), module.GetAt<glm::vec4>(propertyIndex));
    } else if (property.typeName == "quat") {
        glm::quat& quat = module.GetAt<glm::quat>(propertyIndex);
        glm::vec3 eulersDeg(glm::degrees(glm::eulerAngles(quat)));
        glm::vec3 old(eulersDeg);
        std::string title(ICON_FA_EXTERNAL_LINK_SQUARE_ALT " ");
        title.reserve(64);
        title.append(property.prettyName);
        FancyVectorWidget(title.c_str(), eulersDeg, 0);
        quat = quat * glm::quat(glm::radians(eulersDeg - old));
    } else if (property.typeName == "Color") {
        std::string title(ICON_FA_PALETTE " ");
        title.reserve(64);
        title.append(property.prettyName);
        ColorWidget(title.c_str(), module.GetAt<Color>(propertyIndex));
    } else if (property.typeName == "Model") {
        std::string title(ICON_FA_WINE_GLASS " ");
        title.reserve(64);
        title.append(property.prettyName);
        UneditableStringWidget(title.c_str(), module.GetAt<Model*>(propertyIndex)->_name);
    } else if (property.typeName == "Shader") {
        std::string title(ICON_FA_FILE_ALT " ");
        title.reserve(64);
        title.append(property.prettyName);
        UneditableStringWidget(title.c_str(), module.GetAt<Shader*>(propertyIndex)->_name);
    } else {
        std::string text;
        text.reserve(32);
        if (!property.isReference) {
            asIScriptObject*& o = module.GetAt<asIScriptObject*>(propertyIndex);
            if (o == nullptr) {
                text.append("nullptr (");
                text.append(property.typeName);
            }
            else {
                text.append(property.typeName);
                text.append(" (");
                Module m("tm", o);
                auto& mModules = ptr->Modules(m.GetID()); //GetID returns garbage :(
                text.append(mModules["Tag"].As<Tag>().Label());
            }
            text.append(")");
            UneditableStringWidget(property.prettyName.c_str(), text);
            if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonRight)) {
                if (ImGui::MenuItem("Reset")) {
                    o->Release();
                    o = nullptr;
                }
                if (ImGui::MenuItem("Highlight Source")) {
                    highlightedEntity = (Module{ "tm", o }).GetID(); // highlight
                }
                ImGui::EndPopup();
            }
            if (ImGui::BeginDragDropTarget()) {
                auto* payload = ImGui::AcceptDragDropPayload("SELECTED_ENTT");
                if (payload != nullptr) {
                    Entity e = *(Entity*)payload->Data;
                    auto& droppedModules = ptr->Modules(e);
                    for (auto& module : droppedModules._modules) {
                        if (module._name == property.typeName) {
                            o = module;
                            o->AddRef();
                        }
                    }
                }
                ImGui::EndDragDropTarget();
            }
            if (ImGui::IsItemClicked()) {
                highlightedEntity = (Module{ "tm", o }).GetID(); // highlight
            }
        } else {
            std::string name = property.prettyName;
            name.append(" (Read-Only " ICON_FA_LOCK " )");
            Module m("tmp", &module.GetAt<asIScriptObject>(propertyIndex));
            Entity id = m.GetID();
            if (id == NULL_ENTT) {
                text.append("unassigned (");
                text.append(property.typeName);
            }
            else {
                text.append(property.typeName);
                text.append(" (");
                text.append(ptr->Modules(id)["Tag"].As<Tag>().Label());
            }
            text.append(")");
            UneditableStringWidget(name.c_str(), text);
            if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonRight)) {
                if (ImGui::MenuItem("Reset")) {
                    m.SetID(NULL_ENTT);
                }
                if (ImGui::MenuItem("Highlight Source")) {
                    highlightedEntity = m.GetID(); // highlight
                }
                ImGui::EndPopup();
            }
            if (ImGui::BeginDragDropTarget()) {
                auto* payload = ImGui::AcceptDragDropPayload("SELECTED_ENTT");
                if (payload != nullptr) {
                    Entity e = *(Entity*)payload->Data;
                    auto& droppedModules = ptr->Modules(e);
                    for (auto& module : droppedModules._modules) {
                        if (module._name == property.typeName) {
                            m.SetID(module.GetID());
                        }
                    }
                }
                ImGui::EndDragDropTarget();
            }
            if (ImGui::IsItemClicked()) {
                highlightedEntity = m.GetID(); // highlight
            }
        }
    }
}

static void DrawObjectModules(std::unique_ptr<Core>& core, std::weak_ptr<Scene> scene) {
    auto ptr = scene.lock();
    ScriptComponent& scr = ptr->Modules(selectedEntity);
    for (Module& module : scr._modules) {
        asITypeInfo* type = module._module->GetObjectType();
        std::string title = module._name;
        if (!module._isActive) {
            title += " (Deactivated)";
        }
        std::string withIcon = title;
        auto icon = moduleIcons.find(module._name);
        if (icon != moduleIcons.end()) {
            withIcon.insert(0, icon->second);
        } else {
            if (core->_angel->IsDefModule(type)) {
                ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
            }
            withIcon.insert(0, ICON_FA_FILE_CODE " ");
            if (core->_angel->IsDefModule(type)) {
                ImGui::PopFont();
            }
        }
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
        if (!module._isActive) {
            flags |= ImGuiTreeNodeFlags_Leaf;
        }
        if (ImGui::CollapsingHeader(withIcon.c_str(), flags)) {
            if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonRight)) {
                if (!module._isDef) {
                    if (ImGui::MenuItem(ICON_FA_UNLINK " Detach")) {
                        scr.Detach(title.c_str());
                        ImGui::EndPopup();
                        break; // iterator is invalidated!
                    }
                    if (ImGui::MenuItem(module._isActive ? ICON_FA_POWER_OFF " Deactivate" : ICON_FA_POWER_OFF " Activate")) {
                        module._isActive = !module._isActive;
                        ImGui::EndPopup();
                        break; // iterator is invalidated!
                    }
                }
                ImGui::EndPopup();
            }

            if (module._isActive) {
                module.BeforePropertiesGUI();
                std::vector<PropertyData>& props = module.Properties();
                for (int i = 0; i < props.size(); i++) { // starting from 1, the property at 0 is always the "entity" and it's never shown on the editor
                    PropertyData prop = props[i];
                    if(module.OnDrawPropertyGUI(prop, i)) {
                        DrawModulePropertyGUI(module, prop, i);
                    }
                }
                module.AfterPropertiesGUI();
            }
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
        }
    }
}

static void DrawObserverPanel(std::unique_ptr<Core>& core, std::weak_ptr<Scene> scene) {
    auto ready = !scene.expired();
    auto ptr = scene.lock();

    std::ostringstream title;
    if (ready && selectedEntity != NULL_ENTT) {
        ScriptComponent& scr = ptr->Modules(selectedEntity);
        title << scr["Tag"].As<Tag>().Label();
        title << " - ";
    }
    title << "Observer";
    title << "###Observer";
    ImGui::Begin(title.str().c_str(), NULL);
    if(ready) {
        if (selectedEntity != NULL_ENTT) {
            DrawObjectModules(core, scene);
        } else {
            const char* text = "Select an object from the scene :)";
            ImVec2 size = ImGui::GetContentRegionAvail();
            ImVec2 txtSize = ImGui::CalcTextSize(text);
            ImGui::PushStyleColor(ImGuiCol_Text, { ptr->ClearColor.x, ptr->ClearColor.y, ptr->ClearColor.z, 1 });
            for (int i = 0; i < 5; i++) {
                ImGui::SameLine((size.x - txtSize.x) / 2, 0);
                ImGui::Text(text);
                ImGui::NewLine();
            }
            ImGui::PopStyleColor();
        }
    }
    ImGui::End();
}

static void LicenceNotice(const char* name, const char* licence) {
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

static void NewProject(Editor* editor) {
    editor->_openProject->Close();
    editor->_openProject.reset();
    const char* path = tinyfd_selectFolderDialog("Select a folder for New Project", "");
    if (path) {
        const char* name = nullptr;
        bool nameOK = false;
        while (!nameOK) {
            name = tinyfd_inputBox("Enter a name for the New Project", "Enter a name for the New Project", "New Project");
            nameOK = name && std::string(name) != "";
            if (!nameOK) {
                tinyfd_messageBox("Warning", "Projects cannot be unnamed.", "ok", "warning", 1);
            }
        }
        editor->_openProject = std::make_shared<Project>(path, name);
        editor->_openProject->Serialize(path);
        DOA_LOG_INFO("Succesfully created a new project named %s", name);
    }
    ChangeScene(std::weak_ptr<Scene>());
}

static void OpenProject(std::unique_ptr<Core>& core, Editor* editor) {
    editor->_openProject->Close();
    editor->_openProject.reset();
    const char* const types[] = { "*.doa" };
    core->_angel->_scriptLoaderMutex.lock();
    const char* path = tinyfd_openFileDialog("Select Project File", "C:\\", 1, types, "NeoDoa Project Files", 0);
    core->_angel->_scriptLoaderMutex.unlock();
    if (path) {
        editor->_openProject = std::make_shared<Project>(path);
        ChangeScene(editor->_openProject->_startupScene);
    }
}

static void NewScene(FNode* parent, std::string_view name) {
    bool exists = false;
    for (auto& node : parent->_children) {
        if (node._name_noext == name && node._ext == ".scn") {
            exists = true;
            break;
        }
    }
    if (exists) {
        tinyfd_messageBox("Error", "A scene with the same name exists.", "ok", "error", 0);
        return;
    }
    std::stringstream path;
    path << parent->_path;
    path << '\\';
    path << name;
    path << ".scn";

    std::stringstream name_;
    name_ << name;
    name_ << ".scn";
    FNode& node = parent->_children.emplace_back(FNode {
        path.str(),
        name_.str(),
        name.data(),
        ".scn",
        parent,
        {},
        true,
        false
    });

    std::weak_ptr<Scene> scene = CreateScene(name);
    std::ofstream fout(node._path);
    fout << SerializeScene(scene);
    fout.close();
}

static void OpenScene(Editor* editor, FNode* sceneFile) {
    ChangeScene(DeserializeScene(sceneFile));
}

static void SaveScene(Editor* editor, std::weak_ptr<Scene> scene) {
    auto ptr = scene.lock();
    for (FNode*& node : editor->_openProject->_assets._scenes) {
        if (node->_name_noext == ptr->_name) {
            std::ofstream fout(node->_path);
            fout << SerializeScene(scene);
            fout.close();
            return;
        }
    }

}
*/
Editor::Editor() noexcept {
    //neodoaBanner = CreateTexture("!!neodoa!!", "Images/social.png").value();
}

void Editor::operator() (float delta) {
    /*
    static std::unique_ptr<Core>& core = GetCore();
    std::weak_ptr<Scene> scene = FindActiveScene();

    static bool dockspaceOpen = true;
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_NoCloseButton;

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
            if (ImGui::MenuItem("New Project", "Ctrl+Shift+N")) {
                if (_openProject) {
                    if (tinyfd_messageBox("Warning", "You may have unsaved changes. Are you sure you want to create a new project?", "yesno", "warning", 0)) {
                        NewProject(this);
                    }
                } else {
                    NewProject(this);
                }
            }
            if (ImGui::MenuItem("Open Project...", "Ctrl+Shift+O")) {
                if (_openProject) {
                    if (tinyfd_messageBox("Warning", "You may have unsaved changes. Are you sure you want to open another project?", "yesno", "warning", 0)) {
                        OpenProject(core, this);
                    }
                } else {
                    OpenProject(core, this);
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Save Current Scene...", "Ctrl+S")) {
                if (_openProject && !scene.expired()) {
                    SaveScene(this, scene);
                }
            }
            ImGui::Separator();
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


            ImGui::EndPopup();
        }
        ImGui::EndPopup();
    }

    DrawSceneHierarchy(delta, core, scene);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
    ImVec2 viewportSize = DrawViewport(core, scene, delta);
    ImGui::PopStyleVar();

    DrawSceneSettings(core, scene, viewportSize);
    DrawObserverPanel(core, scene);

    DrawConsole();
    DrawAssetManager(this, core, scene);

    ImGui::End();
    */
}

