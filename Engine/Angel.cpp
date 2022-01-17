#include "Angel.hpp"

#include <filesystem>
#include <fstream>

#include <scriptstdstring/scriptstdstring.h>
#include <scriptany/scriptany.h>
#include <scriptarray/scriptarray.h>
#include <scriptdictionary/scriptdictionary.h>
#include <scriptfile/scriptfile.h>
#include <scriptgrid/scriptgrid.h>
#include <scripthandle/scripthandle.h>
#include <scripthelper/scripthelper.h>
#include <scriptmath/scriptmath.h>
#include <datetime/datetime.h>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

#include <imgui.h>

#include <prettify.hpp>

#include "ScriptComponent.hpp"
#include "ScriptStorageComponent.hpp"
#include "Input.hpp"
#include "TypedefsAndConstants.hpp"
#include "Log.hpp"
#include "Scene.hpp"
#include "Mesh.hpp"
#include "Model.hpp"
#include "Shader.hpp"
#include "Color.hpp"

static void logTrace(const std::string& in) { CLI_LOG_TRACE("%s", in.c_str()); }
static void logTrace(int in) { CLI_LOG_TRACE("%d", in); }
static void logTrace(float in) { CLI_LOG_TRACE("%f", in); }
static void logTrace(double in) { CLI_LOG_TRACE("%f", in); }
static void logTrace(bool in) { CLI_LOG_TRACE("%s", in ? "true" : "false"); }
static void logTrace(const glm::vec2& in) { CLI_LOG_TRACE("%s", glm::to_string(in).c_str()); }
static void logTrace(const glm::vec3& in) { CLI_LOG_TRACE("%s", glm::to_string(in).c_str()); }
static void logTrace(const glm::vec4& in) { CLI_LOG_TRACE("%s", glm::to_string(in).c_str()); }

static void logInfo(const std::string& in) { CLI_LOG_INFO("%s", in.c_str()); }
static void logInfo(int in) { CLI_LOG_INFO("%d", in); }
static void logInfo(float in) { CLI_LOG_INFO("%f", in); }
static void logInfo(double in) { CLI_LOG_INFO("%f", in); }
static void logInfo(bool in) { CLI_LOG_INFO("%s", in ? "true" : "false"); }
static void logInfo(const glm::vec2& in) { CLI_LOG_INFO("%s", glm::to_string(in).c_str()); }
static void logInfo(const glm::vec3& in) { CLI_LOG_INFO("%s", glm::to_string(in).c_str()); }
static void logInfo(const glm::vec4& in) { CLI_LOG_INFO("%s", glm::to_string(in).c_str()); }

static void logWarning(const std::string& in) { CLI_LOG_WARNING("%s", in.c_str()); }
static void logWarning(int in) { CLI_LOG_WARNING("%d", in); }
static void logWarning(float in) { CLI_LOG_WARNING("%f", in); }
static void logWarning(double in) { CLI_LOG_WARNING("%f", in); }
static void logWarning(bool in) { CLI_LOG_WARNING("%s", in ? "true" : "false"); }
static void logWarning(const glm::vec2& in) { CLI_LOG_WARNING("%s", glm::to_string(in).c_str()); }
static void logWarning(const glm::vec3& in) { CLI_LOG_WARNING("%s", glm::to_string(in).c_str()); }
static void logWarning(const glm::vec4& in) { CLI_LOG_WARNING("%s", glm::to_string(in).c_str()); }

static void logError(const std::string& in) { CLI_LOG_ERROR("%s", in.c_str()); }
static void logError(int in) { CLI_LOG_ERROR("%d", in); }
static void logError(float in) { CLI_LOG_ERROR("%f", in); }
static void logError(double in) { CLI_LOG_ERROR("%f", in); }
static void logError(bool in) { CLI_LOG_ERROR("%s", in ? "true" : "false"); }
static void logError(const glm::vec2& in) { CLI_LOG_ERROR("%s", glm::to_string(in).c_str()); }
static void logError(const glm::vec3& in) { CLI_LOG_ERROR("%s", glm::to_string(in).c_str()); }
static void logError(const glm::vec4& in) { CLI_LOG_ERROR("%s", glm::to_string(in).c_str()); }

static void logFatal(const std::string& in) { CLI_LOG_FATAL("%s", in.c_str()); }
static void logFatal(int in) { CLI_LOG_FATAL("%d", in); }
static void logFatal(float in) { CLI_LOG_FATAL("%f", in); }
static void logFatal(double in) { CLI_LOG_FATAL("%f", in); }
static void logFatal(bool in) { CLI_LOG_FATAL("%s", in ? "true" : "false"); }
static void logFatal(const glm::vec2& in) { CLI_LOG_FATAL("%s", glm::to_string(in).c_str()); }
static void logFatal(const glm::vec3& in) { CLI_LOG_FATAL("%s", glm::to_string(in).c_str()); }
static void logFatal(const glm::vec4& in) { CLI_LOG_FATAL("%s", glm::to_string(in).c_str()); }

static void vec2Ctor(void* memory) { new(memory) glm::vec2(); }
static void vec2CtorXY(void* memory, float xy) { new(memory) glm::vec2(xy); }
static void vec2CtorX_Y(void* memory, float x, float y) { new(memory) glm::vec2(x, y); }
static void ivec2Ctor(void* memory) { new(memory) glm::ivec2(); }
static void ivec2CtorXY(void* memory, float xy) { new(memory) glm::ivec2(xy); }
static void ivec2CtorX_Y(void* memory, float x, float y) { new(memory) glm::ivec2(x, y); }
static void vec3Ctor(void* memory) { new(memory) glm::vec3(); }
static void vec3CtorXYZ(void* memory, float xyz) { new(memory) glm::vec3(xyz); }
static void vec3CtorX_Y_Z(void* memory, float x, float y, float z) { new(memory) glm::vec3(x, y, z); }
static void ivec3Ctor(void* memory) { new(memory) glm::ivec3(); }
static void ivec3CtorXYZ(void* memory, float xyz) { new(memory) glm::ivec3(xyz); }
static void ivec3CtorX_Y_Z(void* memory, float x, float y, float z) { new(memory) glm::ivec3(x, y, z); }
static void vec4Ctor(void* memory) { new(memory) glm::vec4(); }
static void vec4CtorXYZW(void* memory, float xy) { new(memory) glm::vec4(xy); }
static void vec4CtorX_Y_Z_W(void* memory, float x, float y, float z, float w) { new(memory) glm::vec4(x, y, z, w); }
static void ivec4Ctor(void* memory) { new(memory) glm::ivec4(); }
static void ivec4CtorXYZW(void* memory, float xy) { new(memory) glm::ivec4(xy); }
static void ivec4CtorX_Y_Z_W(void* memory, float x, float y, float z, float w) { new(memory) glm::ivec4(x, y, z, w); }
static void quatCtor(void* memory) { new(memory) glm::quat(); }
static void quatCtorVEC(void* memory, const glm::vec3& v) { new(memory) glm::quat(v); }
static void quatCtor_XYZW(void* memory, float w, float x, float y, float z) { new(memory) glm::quat(w, x, y, z); }

static void vertexCtor(void* memory) { new(memory) Vertex(); }

static void colorCtor(void* memory) { new(memory) Color(); }
static void colorCtorV(void* memory, float v) { new(memory) Color(v); }
static void colorCtorRGB(void* memory, float r, float g, float b) { new(memory) Color(r, g, b); }
static void colorCtorRGBA(void* memory, float r, float g, float b, float a) { new(memory) Color(r, g, b, a); }
static void colorCtorVec3(void* memory, const glm::vec3& color) { new(memory) Color(color); }
static void colorCtorVec4(void* memory, const glm::vec4& color) { new(memory) Color(color); }

static void meshCtor(void* memory) { new(memory) Mesh(std::vector<Vertex>(), std::vector<GLuint>(), std::vector<std::weak_ptr<Texture>>()); }
static void meshDtor(void* memory) { ((Mesh*)memory)->~Mesh(); }

static Model* modelCtor(const std::string& name, std::vector<Mesh>&& meshes) { return &*(CreateModelFromMesh(name, std::move(meshes)).lock()); }

static Shader* findShader(const std::string& name) {
	auto rv = FindShader(name);
	if (!rv.expired()) return &*(rv.lock());
	GetCore()->_angel->_scriptCtx->SetException("Shader not found exception");
	return nullptr;
}
static Model* findModel(const std::string& name) {
	auto rv = FindModel(name);
	if (rv.has_value()) return &*(rv.value().lock());
	GetCore()->_angel->_scriptCtx->SetException("Model not found exception");
	return nullptr;
}

static void MessageCallback(const asSMessageInfo* msg, void* param) {
	if (msg->type == asMSGTYPE_INFORMATION) {
		DOA_LOG_INFO("%s (%d, %d) : %s", msg->section, msg->row, msg->col, msg->message);
	} else if (msg->type == asMSGTYPE_WARNING) {
		DOA_LOG_WARNING("%s (%d, %d) : %s", msg->section, msg->row, msg->col, msg->message);
	} else {
		DOA_LOG_ERROR("%s (%d, %d) : %s", msg->section, msg->row, msg->col, msg->message);
	}
}

Angel::Angel() noexcept :
	_scriptEngine(asCreateScriptEngine()) {
	RegisterStdString(_scriptEngine);			// string
	RegisterScriptArray(_scriptEngine, true);	// array
	RegisterStdStringUtils(_scriptEngine);		// string funcs
	RegisterScriptAny(_scriptEngine);			// any
	RegisterScriptHandle(_scriptEngine);		// ref
	RegisterScriptDictionary(_scriptEngine);	// dict
	RegisterScriptMath(_scriptEngine);			// maths
	RegisterScriptGrid(_scriptEngine);			// natural 2D array
	RegisterScriptDateTime(_scriptEngine);		// date-time
	_scriptEngine->SetEngineProperty(asEEngineProp::asEP_ALLOW_UNSAFE_REFERENCES, true);

#pragma region Register Globals
	int r;
	r = _scriptEngine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL); assert(r >= 0);
#pragma region Input...
	r = _scriptEngine->RegisterGlobalFunction("bool IsKeyPressed(int)", asFUNCTION(IsKeyPressed), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool IsKeyTyped(int)", asFUNCTION(IsKeyTyped), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool IsKeyReleased(int)", asFUNCTION(IsKeyReleased), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool IsMouseButtonPressed(int)", asFUNCTION(IsMouseButtonPressed), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool IsMouseButtonReleased(int button)", asFUNCTION(IsMouseButtonReleased), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("double GetMouseX()", asFUNCTION(GetMouseX), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("double GetMouseY()", asFUNCTION(GetMouseY), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("double GetMouseScrollX()", asFUNCTION(GetMouseScrollX), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("double GetMouseScrollY()", asFUNCTION(GetMouseScrollY), asCALL_CDECL); assert(r >= 0);

	r = _scriptEngine->RegisterGlobalProperty("const int KEY_UNKNOWN", &KEY_UNKNOWN); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_SPACE", &KEY_SPACE); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_APOSTROPHE", &KEY_APOSTROPHE); assert(r >= 0); /* ' */
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_COMMA", &KEY_COMMA); assert(r >= 0); /* , */
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_MINUS", &KEY_MINUS); assert(r >= 0); /* - */
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_PERIOD", &KEY_PERIOD); assert(r >= 0); /* . */
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_SLASH", &KEY_SLASH); assert(r >= 0); /* / */
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_0", &KEY_0); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_1", &KEY_1); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_2", &KEY_2); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_3", &KEY_3); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_4", &KEY_4); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_5", &KEY_5); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_6", &KEY_6); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_7", &KEY_7); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_8", &KEY_8); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_9", &KEY_9); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_SEMICOLON", &KEY_SEMICOLON); assert(r >= 0); /* ; */
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_EQUAL", &KEY_EQUAL); assert(r >= 0); /* = */
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_A", &KEY_A); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_B", &KEY_B); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_C", &KEY_C); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_D", &KEY_D); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_E", &KEY_E); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_F", &KEY_F); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_G", &KEY_G); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_H", &KEY_H); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_I", &KEY_I); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_J", &KEY_J); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_K", &KEY_K); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_L", &KEY_L); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_M", &KEY_M); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_N", &KEY_N); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_O", &KEY_O); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_P", &KEY_P); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_Q", &KEY_Q); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_R", &KEY_R); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_S", &KEY_S); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_T", &KEY_T); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_U", &KEY_U); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_V", &KEY_V); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_W", &KEY_W); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_X", &KEY_X); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_Y", &KEY_Y); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_Z", &KEY_Z); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_LEFT_BRACKET", &KEY_LEFT_BRACKET); assert(r >= 0); /* [ */
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_BACKSLASH", &KEY_BACKSLASH); assert(r >= 0); /* \ */
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_RIGHT_BRACKET", &KEY_RIGHT_BRACKET); assert(r >= 0); /* ] */
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_GRAVE_ACCENT", &KEY_GRAVE_ACCENT); assert(r >= 0); /* ` */
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_WORLD_1", &KEY_WORLD_1); assert(r >= 0); /* non-US #1 */
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_WORLD_2", &KEY_WORLD_2); assert(r >= 0); /* non-US #2 */
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_ESCAPE", &KEY_ESCAPE); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_ENTER", &KEY_ENTER); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_TAB", &KEY_TAB); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_BACKSPACE", &KEY_BACKSPACE); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_INSERT", &KEY_INSERT); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_DELETE", &KEY_DELETE); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_RIGHT", &KEY_RIGHT); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_LEFT", &KEY_LEFT); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_DOWN", &KEY_DOWN); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_UP", &KEY_UP); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_PAGE_UP", &KEY_PAGE_UP); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_PAGE_DOWN", &KEY_PAGE_DOWN); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_HOME", &KEY_HOME); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_END", &KEY_END); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_CAPS_LOCK", &KEY_CAPS_LOCK); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_SCROLL_LOCK", &KEY_SCROLL_LOCK); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_NUM_LOCK", &KEY_NUM_LOCK); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_PRINT_SCREEN", &KEY_PRINT_SCREEN); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_PAUSE", &KEY_PAUSE); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_F1", &KEY_F1); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_F2", &KEY_F2); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_F3", &KEY_F3); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_F4", &KEY_F4); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_F5", &KEY_F5); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_F6", &KEY_F6); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_F7", &KEY_F7); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_F8", &KEY_F8); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_F9", &KEY_F9); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_F10", &KEY_F10); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_F11", &KEY_F11); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_F12", &KEY_F12); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_F13", &KEY_F13); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_F14", &KEY_F14); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_F15", &KEY_F15); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_F16", &KEY_F16); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_F17", &KEY_F17); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_F18", &KEY_F18); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_F19", &KEY_F19); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_F20", &KEY_F20); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_F21", &KEY_F21); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_F22", &KEY_F22); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_F23", &KEY_F23); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_F24", &KEY_F24); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_F25", &KEY_F25); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_KP_0", &KEY_KP_0); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_KP_1", &KEY_KP_1); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_KP_2", &KEY_KP_2); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_KP_3", &KEY_KP_3); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_KP_4", &KEY_KP_4); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_KP_5", &KEY_KP_5); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_KP_6", &KEY_KP_6); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_KP_7", &KEY_KP_7); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_KP_8", &KEY_KP_8); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_KP_9", &KEY_KP_9); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_KP_DECIMAL", &KEY_KP_DECIMAL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_KP_DIVIDE", &KEY_KP_DIVIDE); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_KP_MULTIPLY", &KEY_KP_MULTIPLY); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_KP_SUBTRACT", &KEY_KP_SUBTRACT); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_KP_ADD", &KEY_KP_ADD); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_KP_ENTER", &KEY_KP_ENTER); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_KP_EQUAL", &KEY_KP_EQUAL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_LEFT_SHIFT", &KEY_LEFT_SHIFT); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_LEFT_CONTROL", &KEY_LEFT_CONTROL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_LEFT_ALT", &KEY_LEFT_ALT); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_LEFT_SUPER", &KEY_LEFT_SUPER); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_RIGHT_SHIFT", &KEY_RIGHT_SHIFT); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_RIGHT_CONTROL", &KEY_RIGHT_CONTROL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_RIGHT_ALT", &KEY_RIGHT_ALT); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_RIGHT_SUPER", &KEY_RIGHT_SUPER); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int KEY_MENU", &KEY_MENU); assert(r >= 0);

	r = _scriptEngine->RegisterGlobalProperty("const int MOUSE_BUTTON_1", &MOUSE_BUTTON_1); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int MOUSE_BUTTON_2", &MOUSE_BUTTON_2); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int MOUSE_BUTTON_3", &MOUSE_BUTTON_3); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int MOUSE_BUTTON_4", &MOUSE_BUTTON_4); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int MOUSE_BUTTON_5", &MOUSE_BUTTON_5); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int MOUSE_BUTTON_6", &MOUSE_BUTTON_6); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int MOUSE_BUTTON_7", &MOUSE_BUTTON_7); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int MOUSE_BUTTON_8", &MOUSE_BUTTON_8); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int MOUSE_BUTTON_LEFT", &MOUSE_BUTTON_LEFT); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int MOUSE_BUTTON_RIGHT", &MOUSE_BUTTON_RIGHT); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalProperty("const int MOUSE_BUTTON_MIDDLE", &MOUSE_BUTTON_MIDDLE); assert(r >= 0);
#pragma endregion
#pragma region GLM Types
	RegisterGLMVector<glm::vec2>("vec2");
	r = _scriptEngine->RegisterObjectBehaviour("vec2", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(vec2Ctor), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectBehaviour("vec2", asBEHAVE_CONSTRUCT, "void f(float)", asFUNCTION(vec2CtorXY), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectBehaviour("vec2", asBEHAVE_CONSTRUCT, "void f(float, float)", asFUNCTION(vec2CtorX_Y), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("vec2", "float x", asOFFSET(glm::vec2, x)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("vec2", "float y", asOFFSET(glm::vec2, y)); assert(r >= 0);

	RegisterGLMVector<glm::ivec2>("ivec2");
	r = _scriptEngine->RegisterObjectBehaviour("ivec2", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ivec2Ctor), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectBehaviour("ivec2", asBEHAVE_CONSTRUCT, "void f(int)", asFUNCTION(ivec2CtorXY), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectBehaviour("ivec2", asBEHAVE_CONSTRUCT, "void f(int, int)", asFUNCTION(ivec2CtorX_Y), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("ivec2", "int x", asOFFSET(glm::ivec2, x)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("ivec2", "int y", asOFFSET(glm::ivec2, y)); assert(r >= 0);

	RegisterGLMVector<glm::vec3>("vec3");
	r = _scriptEngine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(vec3Ctor), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f(float)", asFUNCTION(vec3CtorXYZ), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f(float, float, float)", asFUNCTION(vec3CtorX_Y_Z), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("vec3", "float x", asOFFSET(glm::vec3, x)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("vec3", "float y", asOFFSET(glm::vec3, y)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("vec3", "float z", asOFFSET(glm::vec3, z)); assert(r >= 0);

	RegisterGLMVector<glm::ivec3>("ivec3");
	r = _scriptEngine->RegisterObjectBehaviour("ivec3", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ivec3Ctor), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectBehaviour("ivec3", asBEHAVE_CONSTRUCT, "void f(int)", asFUNCTION(ivec3CtorXYZ), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectBehaviour("ivec3", asBEHAVE_CONSTRUCT, "void f(int, int, int)", asFUNCTION(ivec3CtorX_Y_Z), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("ivec3", "int x", asOFFSET(glm::ivec3, x)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("ivec3", "int y", asOFFSET(glm::ivec3, y)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("ivec3", "int z", asOFFSET(glm::ivec3, z)); assert(r >= 0);

	RegisterGLMVector<glm::vec4>("vec4");
	r = _scriptEngine->RegisterObjectBehaviour("vec4", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(vec4Ctor), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectBehaviour("vec4", asBEHAVE_CONSTRUCT, "void f(float)", asFUNCTION(vec4CtorXYZW), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectBehaviour("vec4", asBEHAVE_CONSTRUCT, "void f(float, float, float, float)", asFUNCTION(vec4CtorX_Y_Z_W), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("vec4", "float x", asOFFSET(glm::vec4, x)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("vec4", "float y", asOFFSET(glm::vec4, y)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("vec4", "float z", asOFFSET(glm::vec4, z)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("vec4", "float w", asOFFSET(glm::vec4, w)); assert(r >= 0);

	RegisterGLMVector<glm::ivec4>("ivec4");
	r = _scriptEngine->RegisterObjectBehaviour("ivec4", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ivec4Ctor), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectBehaviour("ivec4", asBEHAVE_CONSTRUCT, "void f(int)", asFUNCTION(ivec4CtorXYZW), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectBehaviour("ivec4", asBEHAVE_CONSTRUCT, "void f(int, int, int, int)", asFUNCTION(ivec4CtorX_Y_Z_W), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("ivec4", "int x", asOFFSET(glm::ivec4, x)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("ivec4", "int y", asOFFSET(glm::ivec4, y)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("ivec4", "int z", asOFFSET(glm::ivec4, z)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("ivec4", "int w", asOFFSET(glm::ivec4, w)); assert(r >= 0);

	r = _scriptEngine->RegisterObjectType("quat", sizeof(glm::quat), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_ALLFLOATS | asGetTypeTraits<glm::quat>()); assert(r >= 0);
	r = _scriptEngine->RegisterObjectBehaviour("quat", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(quatCtor), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectBehaviour("quat", asBEHAVE_CONSTRUCT, "void f(float, float, float, float)", asFUNCTION(quatCtor_XYZW), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectBehaviour("quat", asBEHAVE_CONSTRUCT, "void f(const vec3 &in)", asFUNCTION(quatCtorVEC), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("quat", "float x", asOFFSET(glm::vec4, x)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("quat", "float y", asOFFSET(glm::vec4, y)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("quat", "float z", asOFFSET(glm::vec4, z)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("quat", "float w", asOFFSET(glm::vec4, w)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectMethod("quat", "quat& opAddAssign(const quat &in) const", asMETHODPR(glm::quat, operator+=, (const glm::quat&), glm::quat&), asCALL_THISCALL); assert(r >= 0);
	r = _scriptEngine->RegisterObjectMethod("quat", "quat& opSubAssign(const quat &in) const", asMETHODPR(glm::quat, operator-=, (const glm::quat&), glm::quat&), asCALL_THISCALL); assert(r >= 0);
	r = _scriptEngine->RegisterObjectMethod("quat", "quat& opMulAssign(const quat &in) const", asMETHODPR(glm::quat, operator*=, (const glm::quat&), glm::quat&), asCALL_THISCALL); assert(r >= 0);
	r = _scriptEngine->RegisterObjectMethod("quat", "quat& opAssign(const quat &in) const", asMETHODPR(glm::quat, operator=, (const glm::quat&), glm::quat&), asCALL_THISCALL); assert(r >= 0);
	r = _scriptEngine->RegisterObjectMethod("quat", "quat& opAdd(const quat &in) const", asFUNCTIONPR(glm::operator+, (const glm::quat&, const glm::quat&), glm::quat), asCALL_CDECL_OBJLAST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectMethod("quat", "quat& opSub(const quat &in) const", asFUNCTIONPR(glm::operator-, (const glm::quat&, const glm::quat&), glm::quat), asCALL_CDECL_OBJLAST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectMethod("quat", "quat& opMul(const quat &in) const", asFUNCTIONPR(glm::operator*, (const glm::quat&, const glm::quat&), glm::quat), asCALL_CDECL_OBJLAST); assert(r >= 0);

	r = _scriptEngine->RegisterGlobalFunction("vec2 toRadians(const vec2 &in)", asFUNCTIONPR(glm::radians, (const glm::vec2&), glm::vec2), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("vec3 toRadians(const vec3 &in)", asFUNCTIONPR(glm::radians, (const glm::vec3&), glm::vec3), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("vec4 toRadians(const vec4 &in)", asFUNCTIONPR(glm::radians, (const glm::vec4&), glm::vec4), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("float toRadians(float)", asFUNCTIONPR(glm::radians, (float), float), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("vec2 toDegrees(const vec2 &in)", asFUNCTIONPR(glm::degrees, (const glm::vec2&), glm::vec2), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("vec3 toDegrees(const vec3 &in)", asFUNCTIONPR(glm::degrees, (const glm::vec3&), glm::vec3), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("vec4 toDegrees(const vec4 &in)", asFUNCTIONPR(glm::degrees, (const glm::vec4&), glm::vec4), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("float toDegrees(float)", asFUNCTIONPR(glm::degrees, (float), float), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("vec3 eulerAngles(const quat &in)", asFUNCTIONPR(glm::eulerAngles, (const glm::quat&), glm::vec3), asCALL_CDECL); assert(r >= 0);
#pragma endregion
#pragma region Core Types
	r = _scriptEngine->RegisterObjectType("Vertex", sizeof(Vertex), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<Vertex>()); assert(r >= 0);
	r = _scriptEngine->RegisterObjectBehaviour("Vertex", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(vertexCtor), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("Vertex", "vec3 position", asOFFSET(Vertex, position)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("Vertex", "vec3 normal", asOFFSET(Vertex, normal)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("Vertex", "vec4 color", asOFFSET(Vertex, color)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("Vertex", "vec2 uv", asOFFSET(Vertex, uv)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("Vertex", "int texIndex", asOFFSET(Vertex, texIndex)); assert(r >= 0);

	r = _scriptEngine->RegisterObjectType("Mesh", sizeof(Mesh), asOBJ_VALUE | asGetTypeTraits<Mesh>()); assert(r >= 0);
	r = _scriptEngine->RegisterObjectBehaviour("Mesh", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(meshCtor), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectBehaviour("Mesh", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(meshDtor), asCALL_CDECL_OBJLAST); assert(r >= 0);

	r = _scriptEngine->RegisterObjectType("Model", 0, asOBJ_REF | asOBJ_NOCOUNT); assert(r >= 0);
	r = _scriptEngine->RegisterObjectBehaviour("Model", asBEHAVE_FACTORY, "Model@ f(string &in, array<Mesh> &in)", asFUNCTION(modelCtor), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("Model", "string name", asOFFSET(Model, _name)); assert(r >= 0);

	r = _scriptEngine->RegisterObjectType("Shader", 0, asOBJ_REF | asOBJ_NOCOUNT); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("Shader", "string name", asOFFSET(Shader, _name)); assert(r >= 0);

	r = _scriptEngine->RegisterObjectType("Color", sizeof(Color), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<Color>()); assert(r >= 0);
	r = _scriptEngine->RegisterObjectBehaviour("Color", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(colorCtor), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectBehaviour("Color", asBEHAVE_CONSTRUCT, "void f(float)", asFUNCTION(colorCtorV), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectBehaviour("Color", asBEHAVE_CONSTRUCT, "void f(float, float, float)", asFUNCTION(colorCtorRGB), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectBehaviour("Color", asBEHAVE_CONSTRUCT, "void f(float, float, float, float)", asFUNCTION(colorCtorRGBA), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectBehaviour("Color", asBEHAVE_CONSTRUCT, "void f(const vec3 &in)", asFUNCTION(colorCtorVec3), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectBehaviour("Color", asBEHAVE_CONSTRUCT, "void f(const vec4 &in)", asFUNCTION(colorCtorVec4), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("Color", "float r", asOFFSET(Color, r)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("Color", "float g", asOFFSET(Color, g)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("Color", "float b", asOFFSET(Color, b)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("Color", "float a", asOFFSET(Color, a)); assert(r >= 0);

	r = _scriptEngine->RegisterInterface("__module"); assert(r >= 0);
#pragma endregion
#pragma region Core Functions
	r = _scriptEngine->RegisterGlobalFunction("Shader@ FindShader(const string &in)", asFUNCTION(findShader), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("Model@ FindModel(const string &in)", asFUNCTION(findModel), asCALL_CDECL); assert(r >= 0);

#pragma endregion
#pragma region Print Functions
	r = _scriptEngine->RegisterGlobalFunction("void trace(const string &in)", asFUNCTIONPR(logTrace, (const std::string&), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void trace(int)", asFUNCTIONPR(logTrace, (int), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void trace(float)", asFUNCTIONPR(logTrace, (float), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void trace(double)", asFUNCTIONPR(logTrace, (double), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void trace(bool)", asFUNCTIONPR(logTrace, (bool), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void trace(const vec2 &in)", asFUNCTIONPR(logTrace, (const glm::vec2&), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void trace(const vec3 &in)", asFUNCTIONPR(logTrace, (const glm::vec3&), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void trace(const vec4 &in)", asFUNCTIONPR(logTrace, (const glm::vec4&), void), asCALL_CDECL); assert(r >= 0);

	r = _scriptEngine->RegisterGlobalFunction("void info(const string &in)", asFUNCTIONPR(logInfo, (const std::string&), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void info(int)", asFUNCTIONPR(logInfo, (int), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void info(float)", asFUNCTIONPR(logInfo, (float), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void info(double)", asFUNCTIONPR(logInfo, (double), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void info(bool)", asFUNCTIONPR(logInfo, (bool), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void info(const vec2 &in)", asFUNCTIONPR(logInfo, (const glm::vec2&), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void info(const vec3 &in)", asFUNCTIONPR(logInfo, (const glm::vec3&), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void info(const vec4 &in)", asFUNCTIONPR(logInfo, (const glm::vec4&), void), asCALL_CDECL); assert(r >= 0);

	r = _scriptEngine->RegisterGlobalFunction("void warning(const string &in)", asFUNCTIONPR(logWarning, (const std::string&), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void warning(int)", asFUNCTIONPR(logWarning, (int), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void warning(float)", asFUNCTIONPR(logWarning, (float), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void warning(double)", asFUNCTIONPR(logWarning, (double), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void warning(bool)", asFUNCTIONPR(logWarning, (bool), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void warning(const vec2 &in)", asFUNCTIONPR(logWarning, (const glm::vec2&), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void warning(const vec3 &in)", asFUNCTIONPR(logWarning, (const glm::vec3&), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void warning(const vec4 &in)", asFUNCTIONPR(logWarning, (const glm::vec4&), void), asCALL_CDECL); assert(r >= 0);

	r = _scriptEngine->RegisterGlobalFunction("void error(const string &in)", asFUNCTIONPR(logError, (const std::string&), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void error(int)", asFUNCTIONPR(logError, (int), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void error(float)", asFUNCTIONPR(logError, (float), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void error(double)", asFUNCTIONPR(logError, (double), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void error(bool)", asFUNCTIONPR(logError, (bool), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void error(const vec2 &in)", asFUNCTIONPR(logError, (const glm::vec2&), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void error(const vec3 &in)", asFUNCTIONPR(logError, (const glm::vec3&), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void error(const vec4 &in)", asFUNCTIONPR(logError, (const glm::vec4&), void), asCALL_CDECL); assert(r >= 0);

	r = _scriptEngine->RegisterGlobalFunction("void fatal(const string &in)", asFUNCTIONPR(logFatal, (const std::string&), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void fatal(int)", asFUNCTIONPR(logFatal, (int), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void fatal(float)", asFUNCTIONPR(logFatal, (float), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void fatal(double)", asFUNCTIONPR(logFatal, (double), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void fatal(bool)", asFUNCTIONPR(logFatal, (bool), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void fatal(const vec2 &in)", asFUNCTIONPR(logFatal, (const glm::vec2&), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void fatal(const vec3 &in)", asFUNCTIONPR(logFatal, (const glm::vec3&), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void fatal(const vec4 &in)", asFUNCTIONPR(logFatal, (const glm::vec4&), void), asCALL_CDECL); assert(r >= 0);
#pragma endregion
#pragma region ImGUI Functions
	r = _scriptEngine->SetDefaultNamespace("ImGui"); assert(r >= 0);


	r = _scriptEngine->RegisterGlobalFunction("bool Begin(const string &in, bool, int=0)", asFUNCTIONPR([](const std::string& name, bool opened, int flags) { return ImGui::Begin(name.c_str(), &opened, flags); }, (const std::string&, bool, int), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void End()", asFUNCTIONPR(ImGui::End, (), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool BeginChild(const string &in)", asFUNCTIONPR([](const std::string& name) { return ImGui::Begin(name.c_str()); }, (const std::string&), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void EndChild()", asFUNCTIONPR(ImGui::EndChild, (), void), asCALL_CDECL); assert(r >= 0);


	r = _scriptEngine->RegisterGlobalFunction("vec2 GetContentRegionMax()", asFUNCTIONPR([]() { auto v = ImGui::GetContentRegionMax(); return glm::vec2(v.x, v.y); }, (), glm::vec2), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("vec2 GetContentRegionAvail()", asFUNCTIONPR([]() { auto v = ImGui::GetContentRegionAvail(); return glm::vec2(v.x, v.y); }, (), glm::vec2), asCALL_CDECL); assert(r >= 0);
	//r = _scriptEngine->RegisterGlobalFunction("float GetContentRegionAvailWidth()", asFUNCTIONPR(ImGui::GetContentRegionAvailWidth, (), float), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("vec2 GetWindowContentRegionMin()", asFUNCTIONPR([]() { auto v = ImGui::GetWindowContentRegionMin(); return glm::vec2(v.x, v.y); }, (), glm::vec2), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("vec2 GetWindowContentRegionMax()", asFUNCTIONPR([]() { auto v = ImGui::GetWindowContentRegionMax(); return glm::vec2(v.x, v.y); }, (), glm::vec2), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("float GetWindowRegionWidth()", asFUNCTIONPR(ImGui::GetWindowContentRegionWidth, (), float), asCALL_CDECL); assert(r >= 0);


	r = _scriptEngine->RegisterGlobalFunction("vec2 GetWindowPos()", asFUNCTIONPR([]() { auto v = ImGui::GetWindowPos(); return glm::vec2(v.x, v.y); }, (), glm::vec2), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("vec2 GetWindowSize()", asFUNCTIONPR([]() { auto v = ImGui::GetWindowSize(); return glm::vec2(v.x, v.y); }, (), glm::vec2), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("float GetWindowWidth()", asFUNCTIONPR(ImGui::GetWindowWidth, (), float), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("float GetWindowHeight()", asFUNCTIONPR(ImGui::GetWindowHeight, (), float), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool IsWindowCollapsed()", asFUNCTIONPR(ImGui::IsWindowCollapsed, (), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool IsWindowAppearing()", asFUNCTIONPR(ImGui::IsWindowAppearing, (), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void SetWindowFontScale(float)", asFUNCTIONPR(ImGui::SetWindowFontScale, (float), void), asCALL_CDECL); assert(r >= 0);


	r = _scriptEngine->RegisterGlobalFunction("void SetNextWindowPos(vec2)", asFUNCTIONPR([](glm::vec2 v) { ImGui::SetNextWindowPos(ImVec2(v.x, v.y)); }, (glm::vec2), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void SetNextWindowSize(vec2)", asFUNCTIONPR([](glm::vec2 v) { ImGui::SetNextWindowSize(ImVec2(v.x, v.y)); }, (glm::vec2), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void SetNextWindowContentSize(vec2)", asFUNCTIONPR([](glm::vec2 v) { ImGui::SetNextWindowContentSize(ImVec2(v.x, v.y)); }, (glm::vec2), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void SetNextWindowCollapsed(bool)", asFUNCTIONPR([](bool v) { ImGui::SetNextWindowCollapsed(v); }, (bool), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void SetNextWindowFocus()", asFUNCTIONPR([]() { ImGui::SetNextWindowFocus(); }, (), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void SetWindowPos(vec2)", asFUNCTIONPR([](glm::vec2 v) { ImGui::SetWindowPos(ImVec2(v.x, v.y)); }, (glm::vec2), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void SetWindowSize(vec2)", asFUNCTIONPR([](glm::vec2 v) { ImGui::SetWindowSize(ImVec2(v.x, v.y)); }, (glm::vec2), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void SetWindowCollapsed(bool)", asFUNCTIONPR([](bool v) { ImGui::SetWindowCollapsed(v); }, (bool), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void SetWindowFocus()", asFUNCTIONPR([]() { ImGui::SetWindowFocus(); }, (), void), asCALL_CDECL); assert(r >= 0);


	r = _scriptEngine->RegisterGlobalFunction("void SetWindowPos(const string &in, vec2)", asFUNCTIONPR([](const std::string& name, glm::vec2 v) { ImGui::SetWindowPos(name.c_str(), ImVec2(v.x, v.y)); }, (const std::string&, glm::vec2), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void SetWindowSize(const string &in, vec2)", asFUNCTIONPR([](const std::string& name, glm::vec2 v) { ImGui::SetWindowSize(name.c_str(), ImVec2(v.x, v.y)); }, (const std::string&, glm::vec2), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void SetWindowCollapsed(const string &in, bool)", asFUNCTIONPR([](const std::string& name, bool v) { ImGui::SetWindowCollapsed(name.c_str(), v); }, (const std::string&, bool), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void SetWindowFocus(const string &in)", asFUNCTIONPR([](const std::string& v) { ImGui::SetWindowFocus(v.c_str()); }, (const std::string&), void), asCALL_CDECL); assert(r >= 0);


	r = _scriptEngine->RegisterGlobalFunction("float GetScrollX()", asFUNCTIONPR(ImGui::GetScrollX, (), float), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("float GetScrollY()", asFUNCTIONPR(ImGui::GetScrollY, (), float), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("float GetScrollMaxX()", asFUNCTIONPR(ImGui::GetScrollMaxX, (), float), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("float GetScrollMaxY()", asFUNCTIONPR(ImGui::GetScrollMaxY, (), float), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void SetScrollX(float)", asFUNCTIONPR(ImGui::SetScrollX, (float), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void SetScrollY(float)", asFUNCTIONPR(ImGui::SetScrollY, (float), void), asCALL_CDECL); assert(r >= 0);
	//r = _scriptEngine->RegisterGlobalFunction("void SetScrollHere(float = 0.5f)", asFUNCTIONPR(ImGui::SetScrollHere, (float), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void SetScrollFromPosY(float, float = 0.5f)", asFUNCTIONPR(ImGui::SetScrollFromPosY, (float, float), void), asCALL_CDECL); assert(r >= 0);


	r = _scriptEngine->RegisterGlobalFunction("void Separator()", asFUNCTIONPR(ImGui::Separator, (), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void SameLine(float = 0.0f, float = -1.0f)", asFUNCTIONPR(ImGui::SameLine, (float, float), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void NewLine()", asFUNCTIONPR(ImGui::NewLine, (), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void Spacing()", asFUNCTIONPR(ImGui::Spacing, (), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void Dummy(vec2)", asFUNCTIONPR([](glm::vec2 v) { ImGui::Dummy(ImVec2(v.x, v.y)); }, (glm::vec2), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void Indent(float = 0.0f)", asFUNCTIONPR(ImGui::Indent, (float), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void Unindent(float = 0.0f)", asFUNCTIONPR(ImGui::Unindent, (float), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void BeginGroup()", asFUNCTIONPR(ImGui::BeginGroup, (), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void EndGroup()", asFUNCTIONPR(ImGui::EndGroup, (), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("vec2 GetCursorPos()", asFUNCTIONPR([]() { auto v = ImGui::GetCursorPos(); return glm::vec2(v.x, v.y); }, (), glm::vec2), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("float GetCursorPosX()", asFUNCTIONPR(ImGui::GetCursorPosX, (), float), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("float GetCursorPosY()", asFUNCTIONPR(ImGui::GetCursorPosY, (), float), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void SetCursorPos(vec2)", asFUNCTIONPR([](glm::vec2 v) { ImGui::SetCursorPos(ImVec2(v.x, v.y)); }, (glm::vec2), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void SetCursorPosX(float)", asFUNCTIONPR(ImGui::SetCursorPosX, (float), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void SetCursorPosY(float)", asFUNCTIONPR(ImGui::SetCursorPosY, (float), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("vec2 GetCursorStartPos()", asFUNCTIONPR([]() { auto v = ImGui::GetCursorStartPos(); return glm::vec2(v.x, v.y); }, (), glm::vec2), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("vec2 GetCursorScreenPos()", asFUNCTIONPR([]() { auto v = ImGui::GetCursorScreenPos(); return glm::vec2(v.x, v.y); }, (), glm::vec2), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void SetCursorScreenPos(vec2)", asFUNCTIONPR([](glm::vec2 v) { ImGui::SetCursorScreenPos(ImVec2(v.x, v.y)); }, (glm::vec2), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void AlignTextToFramePadding()", asFUNCTIONPR(ImGui::AlignTextToFramePadding, (), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("float GetTextLineHeight()", asFUNCTIONPR(ImGui::GetTextLineHeight, (), float), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("float GetTextLineHeightWithSpacing()", asFUNCTIONPR(ImGui::GetTextLineHeightWithSpacing, (), float), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("float GetFrameHeight()", asFUNCTIONPR(ImGui::GetFrameHeight, (), float), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("float GetFrameHeightWithSpacing()", asFUNCTIONPR(ImGui::GetFrameHeightWithSpacing, (), float), asCALL_CDECL); assert(r >= 0);

	// Columns
	r = _scriptEngine->RegisterGlobalFunction("void Columns(int = 1, const string &in = string(), bool = true)", asFUNCTIONPR([](int a, const std::string& b, bool c) { ImGui::Columns(a, b != "" ? b.c_str() : 0x0, c);  }, (int, const std::string&, bool), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void NextColumn()", asFUNCTIONPR([]() {  ImGui::NextColumn();  }, (), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("int GetColumnIndex()", asFUNCTIONPR([]() {  return ImGui::GetColumnIndex();  }, (), int), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("float GetColumnWidth(int = -1)", asFUNCTIONPR([](int a) {  return ImGui::GetColumnWidth(a);  }, (int), float), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void SetColumnWidth(int, float)", asFUNCTIONPR([](int a, float b) {  ImGui::SetColumnWidth(a, b);  }, (int, float), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("float GetColumnOffset(int = -1)", asFUNCTIONPR([](int a) {  return ImGui::GetColumnOffset(a);  }, (int), float), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void SetColumnOffset(int, float)", asFUNCTIONPR([](int a, float b) {  ImGui::SetColumnOffset(a, b);  }, (int, float), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("int GetColumnsCount()", asFUNCTIONPR([]() {  return ImGui::GetColumnsCount(); }, (), int), asCALL_CDECL); assert(r >= 0);

	// ID scopes
	r = _scriptEngine->RegisterGlobalFunction("void PushID(const string &in)", asFUNCTIONPR([](const std::string& n) { ImGui::PushID(n.c_str()); }, (const std::string&), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void PushID(int int_id)", asFUNCTIONPR([](int id) { ImGui::PushID(id); }, (int), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void PopID()", asFUNCTIONPR(ImGui::PopID, (), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("uint GetID(const string &in)", asFUNCTIONPR([](const std::string& n) { return ImGui::GetID(n.c_str()); }, (const std::string&), unsigned), asCALL_CDECL); assert(r >= 0);

	// Widgets: Text
	r = _scriptEngine->RegisterGlobalFunction("void Text(const string &in)", asFUNCTIONPR([](const std::string& n) { ImGui::TextUnformatted(n.c_str()); }, (const std::string&), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void TextColored(vec4 col, const string &in)", asFUNCTIONPR([](glm::vec4 c, const std::string& n) { ImGui::TextColored(ImVec4(c.r, c.g, c.b, c.a), n.c_str()); }, (glm::vec4, const std::string&), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void TextWrapped(const string &in)", asFUNCTIONPR([](const std::string& n) { ImGui::TextWrapped(n.c_str()); }, (const std::string&), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void LabelText(const string &in, const string &in)", asFUNCTIONPR([](const std::string& l, const std::string& n) { ImGui::LabelText(l.c_str(), n.c_str()); }, (const std::string&, const std::string&), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void Bullet()", asFUNCTIONPR(ImGui::Bullet, (), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void BulletText(const string &in)", asFUNCTIONPR([](const std::string& n) { ImGui::BulletText(n.c_str()); }, (const std::string&), void), asCALL_CDECL); assert(r >= 0);

	// Widgets: Main
	r = _scriptEngine->RegisterGlobalFunction("bool Button(const string &in, vec2 = vec2(0,0))", asFUNCTIONPR([](const std::string& n, glm::vec2 v) { return ImGui::Button(n.c_str(), ImVec2(v.x, v.y)); }, (const std::string&, glm::vec2), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool SmallButton(const string &in)", asFUNCTIONPR([](const std::string& n) { return ImGui::SmallButton(n.c_str()); }, (const std::string&), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool InvisibleButton(const string &in, vec2)", asFUNCTIONPR([](const std::string& id, glm::vec2 v) { return ImGui::InvisibleButton(id.c_str(), ImVec2(v.x, v.y)); }, (const std::string&, glm::vec2), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void Image(uint, vec2)", asFUNCTIONPR([](unsigned u, glm::vec2 v) { ImGui::Image((ImTextureID)u, ImVec2(v.x, v.y)); }, (unsigned, glm::vec2), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool Checkbox(const string &in, bool &inout)", asFUNCTIONPR([](const std::string& n, bool& v) { return ImGui::Checkbox(n.c_str(), &v); }, (const std::string&, bool&), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool CheckboxFlags(const string &in, uint &inout, uint)", asFUNCTIONPR([](const std::string& n, unsigned& f, unsigned v) { return ImGui::CheckboxFlags(n.c_str(), &f, v); }, (const std::string&, unsigned&, unsigned), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool RadioButton(const string &in, bool)", asFUNCTIONPR([](const std::string& n, bool v) {return ImGui::RadioButton(n.c_str(), v); }, (const std::string&, bool), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool RadioButton(const string &in, int &inout, int)", asFUNCTIONPR([](const std::string& n, int& v, int vv) { return ImGui::RadioButton(n.c_str(), &v, vv); }, (const std::string&, int&, int), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void ProgressBar(float)", asFUNCTIONPR([](float v) { ImGui::ProgressBar(v); }, (float), void), asCALL_CDECL); assert(r >= 0);


	// Widgets: Combo Box
	r = _scriptEngine->RegisterGlobalFunction("bool BeginCombo(const string &in, const string &in, int = 0)", asFUNCTIONPR([](const std::string& id, const std::string& prevItem, int flags) { return ImGui::BeginCombo(id.c_str(), prevItem.c_str(), flags); }, (const std::string&, const std::string&, int), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void EndCombo()", asFUNCTIONPR(ImGui::EndCombo, (), void), asCALL_CDECL); assert(r >= 0);

	static char imgui_comboItem[4096];
	r = _scriptEngine->RegisterGlobalFunction("bool Combo(const string &in, int &inout, const array<string>@+)", asFUNCTIONPR([](const std::string& lbl, int& index, const CScriptArray* items) {
		memset(imgui_comboItem, 0, sizeof(char) * 4096);
		unsigned offset = 0;
		for (unsigned i = 0; i < items->GetSize(); ++i) {
			std::string* str = ((std::string*)items->At(i));
			strcpy(imgui_comboItem + offset, str->c_str());
			offset += str->length() + 1;
		}
		return ImGui::Combo(lbl.c_str(), &index, imgui_comboItem, -1);
	}, (const std::string&, int&, const CScriptArray*), bool), asCALL_CDECL); assert(r >= 0);

	// Widgets: Drags
	r = _scriptEngine->RegisterGlobalFunction("bool DragFloat(const string &in, float &inout, float = 1.0f, float = 0.0f, float = 0.0f)", asFUNCTIONPR([](const std::string& n, float& v, float speed, float mn, float mx) { return ImGui::DragFloat(n.c_str(), &v, speed, mn, mx); }, (const std::string&, float&, float, float, float), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool DragFloat2(const string &in, vec2 &inout)", asFUNCTIONPR([](const std::string& n, glm::vec2& v) { return ImGui::DragFloat2(n.c_str(), &v.x); }, (const std::string&, glm::vec2&), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool DragFloat3(const string &in, vec3 &inout)", asFUNCTIONPR([](const std::string& n, glm::vec3& v) { return ImGui::DragFloat3(n.c_str(), &v.x); }, (const std::string&, glm::vec3&), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool DragFloat4(const string &in, vec4 &inout)", asFUNCTIONPR([](const std::string& n, glm::vec4& v) { return ImGui::DragFloat4(n.c_str(), &v.x); }, (const std::string&, glm::vec4&), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool DragInt(const string &in, int &inout, int = 0, int = 0)", asFUNCTIONPR([](const std::string& n, int& v, int mn, int mx) { return ImGui::DragInt(n.c_str(), &v, 1.0f, mn, mx); }, (const std::string&, int&, int, int), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool DragInt2(const string &in, ivec2 &inout, int = 0, int = 0)", asFUNCTIONPR([](const std::string& n, glm::ivec2& v, int mn, int mx) { return ImGui::DragInt2(n.c_str(), &v.x, 1.0f, mn, mx); }, (const std::string&, glm::ivec2&, int, int), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool DragInt3(const string &in, ivec3 &inout, int = 0, int = 0)", asFUNCTIONPR([](const std::string& n, glm::ivec3& v, int mn, int mx) { return ImGui::DragInt3(n.c_str(), &v.x, 1.0f, mn, mx); }, (const std::string&, glm::ivec3&, int, int), bool), asCALL_CDECL); assert(r >= 0);

	r = _scriptEngine->RegisterGlobalFunction("bool DragFloatRange2(const string &in, float &inout, float &inout, float = 0.0f, float = 1.0f)", asFUNCTIONPR([](const std::string& n, float& v0, float& v1, float mn, float mx) { return ImGui::DragFloatRange2(n.c_str(), &v0, &v1, 1.0f, mn, mx); }, (const std::string&, float&, float&, float, float), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool DragIntRange2(const string &in, int &inout, int &inout, int, int)", asFUNCTIONPR([](const std::string& n, int& v0, int& v1, int mn, int mx) { return ImGui::DragIntRange2(n.c_str(), &v0, &v1, 1.0f, mn, mx); }, (const std::string&, int&, int&, int, int), bool), asCALL_CDECL); assert(r >= 0);

	// Widgets: Input with Keyboard
	static char imgui_text_buffer[4096]; // shared with multiple widgets
	r = _scriptEngine->RegisterGlobalFunction("bool InputText(const string &in, string &inout)", asFUNCTIONPR([](const std::string& id, std::string& val) {
		memset(imgui_text_buffer, 0, sizeof(char) * 4096);
		strcpy(imgui_text_buffer, val.c_str());
		if (ImGui::InputText(id.c_str(), imgui_text_buffer, 4096)) {
			val = imgui_text_buffer;
			return true;
		}
		return false;
	}, (const std::string&, std::string&), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool InputTextMultiline(const string &in, string &inout)", asFUNCTIONPR([](const std::string& id, std::string& val) {
		memset(imgui_text_buffer, 0, sizeof(char) * 4096);
		strcpy(imgui_text_buffer, val.c_str());
		if (ImGui::InputTextMultiline(id.c_str(), imgui_text_buffer, 4096)) {
			val = imgui_text_buffer;
			return true;
		}
		return false;
	}, (const std::string&, std::string&), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool InputFloat(const string&, float &inout)", asFUNCTIONPR([](const std::string& id, float& val) { return ImGui::InputFloat(id.c_str(), &val); }, (const std::string&, float&), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool InputFloat2(const string&, vec2 &inout)", asFUNCTIONPR([](const std::string& id, glm::vec2& val) { return ImGui::InputFloat2(id.c_str(), &val.x); }, (const std::string&, glm::vec2&), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool InputFloat3(const string&, vec3 &inout)", asFUNCTIONPR([](const std::string& id, glm::vec3& val) { return ImGui::InputFloat3(id.c_str(), &val.x); }, (const std::string&, glm::vec3&), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool InputFloat4(const string&, vec4 &inout)", asFUNCTIONPR([](const std::string& id, glm::vec4& val) { return ImGui::InputFloat4(id.c_str(), &val.x); }, (const std::string&, glm::vec4&), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool InputInt(const string&, int &inout)", asFUNCTIONPR([](const std::string& id, int& val) { return ImGui::InputInt(id.c_str(), &val); }, (const std::string&, int&), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool InputInt2(const string&, ivec2 &inout)", asFUNCTIONPR([](const std::string& id, glm::ivec2& val) { return ImGui::InputInt2(id.c_str(), &val.x); }, (const std::string&, glm::ivec2&), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool InputInt3(const string&, ivec3 &inout)", asFUNCTIONPR([](const std::string& id, glm::ivec3& val) { return ImGui::InputInt3(id.c_str(), &val.x); }, (const std::string&, glm::ivec3&), bool), asCALL_CDECL); assert(r >= 0);

	// Widgets: Sliders (tip: ctrl+click on a slider to input with keyboard. manually input values aren't clamped, can go off-bounds)
	r = _scriptEngine->RegisterGlobalFunction("bool SliderFloat(const string &in, float &inout, float = 0.0f, float = 0.0f)", asFUNCTIONPR([](const std::string& n, float& v, float mn, float mx) { return ImGui::SliderFloat(n.c_str(), &v, mn, mx); }, (const std::string&, float&, float, float), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool SliderFloat2(const string &in, vec2 &inout, float, float)", asFUNCTIONPR([](const std::string& n, glm::vec2& v, float mn, float mx) { return ImGui::SliderFloat2(n.c_str(), &v.x, mn, mx); }, (const std::string&, glm::vec2&, float, float), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool SliderFloat3(const string &in, vec3 &inout, float, float)", asFUNCTIONPR([](const std::string& n, glm::vec3& v, float mn, float mx) { return ImGui::SliderFloat3(n.c_str(), &v.x, mn, mx); }, (const std::string&, glm::vec3&, float, float), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool SliderFloat4(const string &in, vec4 &inout, float, float)", asFUNCTIONPR([](const std::string& n, glm::vec4& v, float mn, float mx) { return ImGui::SliderFloat4(n.c_str(), &v.x, mn, mx); }, (const std::string&, glm::vec4&, float, float), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool SliderInt(const string &in, int &inout, int = 0, int = 0)", asFUNCTIONPR([](const std::string& n, int& v, int mn, int mx) { return ImGui::SliderInt(n.c_str(), &v, mn, mx); }, (const std::string&, int&, int, int), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool SliderInt2(const string &in, ivec2 &inout, int = 0, int = 0)", asFUNCTIONPR([](const std::string& n, glm::ivec2& v, int mn, int mx) { return ImGui::SliderInt2(n.c_str(), &v.x, mn, mx); }, (const std::string&, glm::ivec2&, int, int), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool SliderInt3(const string &in, ivec3 &inout, int = 0, int = 0)", asFUNCTIONPR([](const std::string& n, glm::ivec3& v, int mn, int mx) { return ImGui::SliderInt3(n.c_str(), &v.x, mn, mx); }, (const std::string&, glm::ivec3&, int, int), bool), asCALL_CDECL); assert(r >= 0);

	// Widgets: Color Editor/Picker
	r = _scriptEngine->RegisterGlobalFunction("bool ColorEdit3(const string &in, Color &inout)", asFUNCTIONPR([](const std::string& id, Color& val) {
		auto v = val.ToVec3();
		if (ImGui::ColorEdit3(id.c_str(), &v.r)) {
			val = Color(v.r, v.g, v.b);
			return true;
		}
		return false;
	}, (const std::string&, Color&), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool ColorEdit4(const string &in, Color &inout)", asFUNCTIONPR([](const std::string& id, Color& val) {
		auto v = val.ToVec4();
		if (ImGui::ColorEdit4(id.c_str(), &v.r)) {
			val = Color(v.r, v.g, v.b, v.a);
			return true;
		}
		return false;
	}, (const std::string&, Color&), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool ColorPicker3(const string &in, Color &inout)", asFUNCTIONPR([](const std::string& id, Color& val) {
		auto v = val.ToVec3();
		if (ImGui::ColorPicker3(id.c_str(), &v.r)) {
			val = Color(v.r, v.g, v.b);
			return true;
		}
		return false;
	}, (const std::string&, Color&), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool ColorPicker4(const string &in, Color &inout)", asFUNCTIONPR([](const std::string& id, Color& val) {
		auto v = val.ToVec4();
		if (ImGui::ColorPicker4(id.c_str(), &v.r)) {
			val = Color(v.r, v.g, v.b, v.a);
			return true;
		}
		return false;
	}, (const std::string&, Color&), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool ColorButton(const string &in, Color)", asFUNCTIONPR([](const std::string& id, Color val) {
		auto v = val.ToVec4();
		ImVec4 vv(v.r, v.g, v.b, v.a);
		return ImGui::ColorButton(id.c_str(), vv);
	}, (const std::string&, Color), bool), asCALL_CDECL); assert(r >= 0);

	// Widgets: Trees
	r = _scriptEngine->RegisterGlobalFunction("bool TreeNode(const string &in)", asFUNCTIONPR([](const std::string& id) { return ImGui::TreeNode(id.c_str()); }, (const std::string&), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void TreePush(const string &in)", asFUNCTIONPR([](const std::string& id) { ImGui::TreePush(id.c_str()); }, (const std::string&), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void TreePop()", asFUNCTIONPR(ImGui::TreePop, (), void), asCALL_CDECL); assert(r >= 0);
	//r = _scriptEngine->RegisterGlobalFunction("void TreeAdvanceToLabelPos()", asFUNCTIONPR(ImGui::TreeAdvanceToLabelPos, (), void), asCALL_CDECL); assert(r >= 0);
	//r = _scriptEngine->RegisterGlobalFunction("float GetTreeNodeToLabelSpacing()", asFUNCTIONPR(ImGui::GetTreeNodeToLabelSpacing, (), float), asCALL_CDECL); assert(r >= 0);
	//r = _scriptEngine->RegisterGlobalFunction("void SetNextTreeNodeOpen(bool)", asFUNCTIONPR([](bool val) { ImGui::SetNextTreeNodeOpen(val); }, (bool), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool CollapsingHeader(const string &in)", asFUNCTIONPR([](const std::string& n) { return ImGui::CollapsingHeader(n.c_str()); }, (const std::string&), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool CollapsingHeader(const string &in, bool &inout)", asFUNCTIONPR([](const std::string& n, bool& v) { return ImGui::CollapsingHeader(n.c_str(), &v); }, (const std::string&, bool&), bool), asCALL_CDECL); assert(r >= 0);

	// Widgets: Selectable / Lists
	r = _scriptEngine->RegisterGlobalFunction("bool Selectable(const string &in, bool = false)", asFUNCTIONPR([](const std::string& n, bool v) { return ImGui::Selectable(n.c_str(), v); }, (const std::string&, bool), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool ListBoxHeader(const string &in)", asFUNCTIONPR([](const std::string& n) { return ImGui::ListBoxHeader(n.c_str()); }, (const std::string&), bool), asCALL_CDECL); assert(r >= 0);

	// Values
	r = _scriptEngine->RegisterGlobalFunction("void Value(const string &in, bool)", asFUNCTIONPR([](const std::string& n, bool v) { ImGui::Value(n.c_str(), v); }, (const std::string&, bool), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void Value(const string &in, int)", asFUNCTIONPR([](const std::string& n, int v) { ImGui::Value(n.c_str(), v); }, (const std::string&, int), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void Value(const string &in, uint)", asFUNCTIONPR([](const std::string& n, unsigned v) { ImGui::Value(n.c_str(), v); }, (const std::string&, unsigned), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void Value(const string &in, float)", asFUNCTIONPR([](const std::string& n, float v) { ImGui::Value(n.c_str(), v); }, (const std::string&, float), void), asCALL_CDECL); assert(r >= 0);

	// Tooltips
	r = _scriptEngine->RegisterGlobalFunction("void BeginTooltip()", asFUNCTIONPR(ImGui::BeginTooltip, (), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void EndTooltip()", asFUNCTIONPR(ImGui::EndTooltip, (), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void SetTooltip(const string &in)", asFUNCTIONPR([](const std::string& t) { ImGui::SetTooltip(t.c_str()); }, (const std::string&), void), asCALL_CDECL); assert(r >= 0);

	// Menus
	r = _scriptEngine->RegisterGlobalFunction("bool BeginMainMenuBar()", asFUNCTIONPR([]() { return ImGui::BeginMainMenuBar(); }, (), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void EndMainMenuBar()", asFUNCTIONPR([]() { ImGui::EndMainMenuBar(); }, (), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool BeginMenuBar()", asFUNCTIONPR([]() { return ImGui::BeginMenuBar(); }, (), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void EndMenuBar()", asFUNCTIONPR([]() { ImGui::EndMenuBar(); }, (), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool BeginMenu(const string &in, bool = true)", asFUNCTIONPR([](const std::string& a, bool b) { return ImGui::BeginMenu(a != "" ? a.c_str() : 0x0, b);  }, (const std::string&, bool), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void EndMenu()", asFUNCTIONPR([]() { ImGui::EndMenu(); }, (), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool MenuItem(const string &in, const string &in = string(), bool = false, bool = true)", asFUNCTIONPR([](const std::string& a, const std::string& b, bool c, bool d) { return ImGui::MenuItem(a != "" ? a.c_str() : 0x0, b != "" ? b.c_str() : 0x0, c, d); }, (const std::string&, const std::string&, bool, bool), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool MenuItem(const string &in, const string &in, bool &inout, bool = true)", asFUNCTIONPR([](const std::string& a, const std::string& b, bool& c, bool d) { return ImGui::MenuItem(a != "" ? a.c_str() : 0x0, b != "" ? b.c_str() : 0x0, &c, d); }, (const std::string&, const std::string&, bool&, bool), bool), asCALL_CDECL); assert(r >= 0);

	// Popups
	r = _scriptEngine->RegisterGlobalFunction("void OpenPopup(const string &in)", asFUNCTIONPR([](const std::string& a) { ImGui::OpenPopup(a != "" ? a.c_str() : 0x0); }, (const std::string&), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool BeginPopup(const string &in, int = 0)", asFUNCTIONPR([](const std::string& a, int b) { return ImGui::BeginPopup(a != "" ? a.c_str() : 0x0, (ImGuiWindowFlags)b); }, (const std::string&, int), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool BeginPopupContextItem(const string &in = string(), int = 1)", asFUNCTIONPR([](const std::string& a, int b) { return ImGui::BeginPopupContextItem(a != "" ? a.c_str() : 0x0, b); }, (const std::string&, int), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool BeginPopupContextWindow(const string &in = string(), int = 1, bool = true)", asFUNCTIONPR([](const std::string& a, int b, bool c) { return ImGui::BeginPopupContextWindow(a != "" ? a.c_str() : 0x0, b, c); }, (const std::string&, int, bool), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool BeginPopupContextVoid(const string &in = string(), int = 1)", asFUNCTIONPR([](const std::string& a, int b) { return ImGui::BeginPopupContextVoid(a != "" ? a.c_str() : 0x0, b); }, (const std::string&, int), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool BeginPopupModal(const string &in, bool &inout = null, int = 0)", asFUNCTIONPR([](const std::string& a, bool& b, int c) { return ImGui::BeginPopupModal(a != "" ? a.c_str() : 0x0, &b, (ImGuiWindowFlags)c); }, (const std::string&, bool&, int), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void EndPopup()", asFUNCTIONPR([]() {  ImGui::EndPopup();  }, (), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void OpenPopupOnItemClick(const string &in = string(), int = 1)", asFUNCTIONPR([](const std::string& a, int b) { ImGui::OpenPopupOnItemClick(a != "" ? a.c_str() : 0x0, b); }, (const std::string&, int), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool IsPopupOpen(const string &in)", asFUNCTIONPR([](const std::string& a) { return ImGui::IsPopupOpen(a != "" ? a.c_str() : 0x0); }, (const std::string&), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void CloseCurrentPopup()", asFUNCTIONPR([]() { ImGui::CloseCurrentPopup(); }, (), void), asCALL_CDECL); assert(r >= 0);

	// Clip-rects
	r = _scriptEngine->RegisterGlobalFunction("void PushClipRect(const vec2&, const vec2&, bool)", asFUNCTIONPR([](const glm::vec2& a, const glm::vec2& b, bool c) { ImGui::PushClipRect(ImVec2(a.x, a.y), ImVec2(b.x, b.y), c); }, (const glm::vec2&, const glm::vec2&, bool), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void PopClipRect()", asFUNCTIONPR([]() { ImGui::PopClipRect(); }, (), void), asCALL_CDECL); assert(r >= 0);

	// Focus
	r = _scriptEngine->RegisterGlobalFunction("void SetItemDefaultFocus()", asFUNCTIONPR([]() { ImGui::SetItemDefaultFocus(); }, (), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void SetKeyboardFocusHere(int = 0)", asFUNCTIONPR([](int a) { ImGui::SetKeyboardFocusHere(a); }, (int), void), asCALL_CDECL); assert(r >= 0);

	// Utilities
	r = _scriptEngine->RegisterGlobalFunction("bool IsItemHovered(int = 0)", asFUNCTIONPR([](int a) { return ImGui::IsItemHovered((ImGuiHoveredFlags)a); }, (int), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool IsItemActive()", asFUNCTIONPR([]() { return ImGui::IsItemActive(); }, (), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool IsItemClicked(int = 0)", asFUNCTIONPR([](int a) { return ImGui::IsItemClicked(a); }, (int), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool IsItemVisible()", asFUNCTIONPR([]() { return ImGui::IsItemVisible(); }, (), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool IsAnyItemHovered()", asFUNCTIONPR([]() { return ImGui::IsAnyItemHovered(); }, (), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool IsAnyItemActive()", asFUNCTIONPR([]() { return ImGui::IsAnyItemActive(); }, (), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("vec2 GetItemRectMin()", asFUNCTIONPR([]() { auto v = ImGui::GetItemRectMin(); return glm::vec2(v.x, v.y); }, (), glm::vec2), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("vec2 GetItemRectMax()", asFUNCTIONPR([]() { auto v = ImGui::GetItemRectMax(); return glm::vec2(v.x, v.y); }, (), glm::vec2), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("vec2 GetItemRectSize()", asFUNCTIONPR([]() { auto v = ImGui::GetItemRectSize(); return glm::vec2(v.x, v.y); }, (), glm::vec2), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void SetItemAllowOverlap()", asFUNCTIONPR([]() {  ImGui::SetItemAllowOverlap(); }, (), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool IsWindowFocused(int = 0)", asFUNCTIONPR([](int a) { return ImGui::IsWindowFocused((ImGuiFocusedFlags)a); }, (int), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool IsWindowHovered(int = 0)", asFUNCTIONPR([](int a) { return ImGui::IsWindowHovered((ImGuiHoveredFlags)a); }, (int), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool IsRectVisible(const vec2&)", asFUNCTIONPR([](const glm::vec2& a) { return ImGui::IsRectVisible(ImVec2(a.x, a.y)); }, (const glm::vec2&), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool IsRectVisible(const vec2&, const vec2&)", asFUNCTIONPR([](const glm::vec2& a, const glm::vec2& b) { return ImGui::IsRectVisible(ImVec2(a.x, a.y), ImVec2(b.x, b.y)); }, (const glm::vec2&, const glm::vec2&), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("float GetTime()", asFUNCTIONPR([]() { return (float)ImGui::GetTime(); }, (), float), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("int GetFrameCount()", asFUNCTIONPR([]() { return ImGui::GetFrameCount(); }, (), int), asCALL_CDECL); assert(r >= 0);

	r = _scriptEngine->RegisterGlobalFunction("vec2 CalcTextSize(const string &in, const string &in = string(), bool = false, float = -1.0f)", asFUNCTIONPR([](const std::string& a, const std::string& b, bool c, float d) { auto v = ImGui::CalcTextSize(a != "" ? a.c_str() : 0x0, b != "" ? b.c_str() : 0x0, c, d); return glm::vec2(v.x, v.y); }, (const std::string&, const std::string&, bool, float), glm::vec2), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void CalcListClipping(int, float, int &inout, int &inout)", asFUNCTIONPR([](int a, float b, int& c, int& d) { glm:ImGui::CalcListClipping(a, b, &c, &d);  }, (int, float, int&, int&), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool BeginChildFrame(uint, const vec2&, int = 0)", asFUNCTIONPR([](unsigned a, const glm::vec2& b, int c) { return ImGui::BeginChildFrame(a, ImVec2(b.x, b.y), (ImGuiWindowFlags)c); }, (unsigned, const glm::vec2&, int), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void EndChildFrame()", asFUNCTIONPR([]() { ImGui::EndChildFrame(); }, (), void), asCALL_CDECL); assert(r >= 0);

	r = _scriptEngine->RegisterGlobalFunction("int GetKeyIndex(int)", asFUNCTIONPR([](int a) { return ImGui::GetKeyIndex((ImGuiKey)a); }, (int), int), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool IsKeyDown(int)", asFUNCTIONPR([](int a) { return ImGui::IsKeyDown(a); }, (int), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool IsKeyPressed(int, bool = true)", asFUNCTIONPR([](int a, bool b) { return ImGui::IsKeyPressed(a, b); }, (int, bool), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool IsKeyReleased(int)", asFUNCTIONPR([](int a) { return ImGui::IsKeyReleased(a); }, (int), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("int GetKeyPressedAmount(int, float, float)", asFUNCTIONPR([](int a, float b, float c) { return ImGui::GetKeyPressedAmount(a, b, c); }, (int, float, float), int), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool IsMouseDown(int)", asFUNCTIONPR([](int a) { return ImGui::IsMouseDown(a); }, (int), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool IsMouseClicked(int, bool = false)", asFUNCTIONPR([](int a, bool b) { return ImGui::IsMouseClicked(a, b); }, (int, bool), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool IsMouseDoubleClicked(int)", asFUNCTIONPR([](int a) { return ImGui::IsMouseDoubleClicked(a); }, (int), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool IsMouseReleased(int)", asFUNCTIONPR([](int a) { return ImGui::IsMouseReleased(a); }, (int), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool IsMouseDragging(int = 0, float = -1.0f)", asFUNCTIONPR([](int a, float b) { return ImGui::IsMouseDragging(a, b); }, (int, float), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool IsMouseHoveringRect(const vec2 &in, const vec2 &in, bool = true)", asFUNCTIONPR([](const glm::vec2& a, const glm::vec2& b, bool c) { return ImGui::IsMouseHoveringRect(ImVec2(a.x, a.y), ImVec2(b.x, b.y), c);  }, (const glm::vec2&, const glm::vec2&, bool), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool IsMousePosValid(const vec2 &in)", asFUNCTIONPR([](const glm::vec2& a) { auto v = ImVec2(a.x, a.y); return ImGui::IsMousePosValid(&v); }, (const glm::vec2&), bool), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("vec2 GetMousePos()", asFUNCTIONPR([]() { auto v = ImGui::GetMousePos(); return glm::vec2(v.x, v.y); }, (), glm::vec2), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("vec2 GetMousePosOnOpeningCurrentPopup()", asFUNCTIONPR([]() { auto v = ImGui::GetMousePosOnOpeningCurrentPopup(); return glm::vec2(v.x, v.y); }, (), glm::vec2), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("vec2 GetMouseDragDelta(int = 0, float = -1.0f)", asFUNCTIONPR([](int a, float b) { auto v = ImGui::GetMouseDragDelta(a, b); return glm::vec2(v.x, v.y); }, (int, float), glm::vec2), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void ResetMouseDragDelta(int = 0)", asFUNCTIONPR([](int a) { ImGui::ResetMouseDragDelta(a); }, (int), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("int GetMouseCursor()", asFUNCTIONPR([]() { return ImGui::GetMouseCursor(); }, (), int), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void SetMouseCursor(int)", asFUNCTIONPR([](ImGuiMouseCursor a) { ImGui::SetMouseCursor(a); }, (int), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void CaptureKeyboardFromApp(bool = true)", asFUNCTIONPR([](bool a) { ImGui::CaptureKeyboardFromApp(a); }, (bool), void), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void CaptureMouseFromApp(bool = true)", asFUNCTIONPR([](bool a) { ImGui::CaptureMouseFromApp(a); }, (bool), void), asCALL_CDECL); assert(r >= 0);


	r = _scriptEngine->RegisterGlobalFunction("string GetClipboardText()", asFUNCTIONPR([]() { return std::string(ImGui::GetClipboardText()); }, (), std::string), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void SetClipboardText(const string &in)", asFUNCTIONPR([](const std::string& a) { ImGui::SetClipboardText(a != "" ? a.c_str() : 0x0); }, (const std::string&), void), asCALL_CDECL); assert(r >= 0);

	r = _scriptEngine->SetDefaultNamespace(""); assert(r >= 0);
#pragma endregion
#pragma region Auxillary Types
	r = _scriptEngine->RegisterObjectType("PropertyData", sizeof(PropertyData), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<PropertyData>()); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("PropertyData", "string name", asOFFSET(PropertyData, name)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("PropertyData", "string typeName", asOFFSET(PropertyData, typeName)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("PropertyData", "int typeId", asOFFSET(PropertyData, typeId)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("PropertyData", "bool isPrivate", asOFFSET(PropertyData, isPrivate)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("PropertyData", "bool isProtected", asOFFSET(PropertyData, isProtected)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("PropertyData", "bool offset", asOFFSET(PropertyData, offset)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("PropertyData", "bool isReference", asOFFSET(PropertyData, isReference)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("PropertyData", "int compositeOffset", asOFFSET(PropertyData, compositeOffset)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("PropertyData", "bool isCompositeIndirect", asOFFSET(PropertyData, isCompositeIndirect)); assert(r >= 0);
#pragma endregion
#pragma endregion

	_scriptCtx = _scriptEngine->CreateContext();
	FindAndBuildScripts();
	_scriptLoaderDeamon = std::thread([&]() {
		while (_scriptLoaderRunning) {
			_scriptLoaderMutex.lock();
			FindAndBuildScripts();
			_scriptLoaderMutex.unlock();
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1s);
		}
	});
}

Angel::~Angel() noexcept {
	_scriptLoaderRunning = false;

	for (auto& ctor : _scriptComponentCtors) {
		ctor.second->Release();
	}

	_scriptLoaderDeamon.join();
	_scriptModule->Discard();
	_scriptCtx->Release();
	_scriptEngine->ShutDownAndRelease();
}

ScriptComponent Angel::InstantiateScriptComponentIncomplete(std::string_view componentType) {
	if (_scriptComponentCtors.count(componentType.data()) == 0) {
		DOA_LOG_ERROR("No such script component defined!");
		throw;
	}
	_scriptCtx->Prepare(_scriptComponentCtors[componentType.data()]);
	_scriptCtx->Execute();
	asIScriptObject* obj = *(asIScriptObject**)_scriptCtx->GetAddressOfReturnValue();
	ScriptComponent m;
	m._name = componentType;
	m._underlyingInstance = obj;
	return m;
}

void Angel::FindAndBuildScripts() {
	std::string path = "Scripts"; // TODO change this!
	std::vector<std::string> files;
	std::vector<std::string> filesContent;
	auto it = std::filesystem::recursive_directory_iterator(path, std::filesystem::directory_options::skip_permission_denied);
	for (const auto& entry : it) {
		if (!entry.is_directory() && entry.path().extension().string() == ".as") {
			files.emplace_back(entry.path().string());
			std::ifstream scr;
			scr.open(entry.path().string());
			std::stringstream scrSs;
			scrSs << scr.rdbuf();
			filesContent.emplace_back(scrSs.str());
		}
	}
	if (files != _scriptFiles || filesContent != _scriptFilesContent) {
		DOA_LOG_TRACE("Found a new file! Rebuilding!");
		_scriptFiles = std::move(files);
		_scriptFilesContent = std::move(filesContent);
		if (Rebuild()) {
			DOA_LOG_TRACE("Script Database Succesfully Built!");
		}
	}
}

bool Angel::Rebuild() {
	if (_scriptBuilder.StartNewModule(_scriptEngine, "Scripts") < 0) {
		// If the code fails here it is usually because there
		// is no more memory to allocate the module, buy more ram maybeeeeee
		DOA_LOG_ERROR("Unrecoverable error while starting a new module. Most possible cause is insufficient memory.");
		return false;
	}
	_scriptModule = _scriptEngine->GetModule("Scripts");

	for (auto i = 0; i < _scriptFiles.size(); i++) {
		if (_scriptBuilder.AddSectionFromMemory(_scriptFiles[i].c_str(), _scriptFilesContent[i].c_str()) < 0) {
			// The builder wasn't able to load the file. Maybe the file
			// has been removed, or the wrong name was given, or some
			// preprocessing commands are incorrectly written.
			DOA_LOG_ERROR("Please correct the errors in %s and try again.", _scriptFiles[i]);
			return false;
		}
	}
	if (_scriptBuilder.BuildModule() < 0) {
		// An error occurred. Instruct the script writer to fix the
		// compilation errors that were listed in the output stream.
		DOA_LOG_ERROR("Script compilation failed!");
		return false;
	}

	{ //reset everything
		_scriptComponentData.clear();

		for (auto& ctor : _scriptComponentCtors) {
			ctor.second->Release();
		}
		_scriptComponentCtors.clear();
	}

	{//enumerate and store all components and systems
		int classCount = _scriptModule->GetObjectTypeCount(); // get the number of all types
		for (int i = 0; i < classCount; i++) { // for each type
			asITypeInfo* type = _scriptModule->GetObjectTypeByIndex(i);
			const char* name = type->GetName();

			DOA_LOG_INFO("Script: %s succesfully registered", name); // print its name
			int fieldCount = type->GetPropertyCount(); // and all of its fields...
			_scriptComponentData.insert({ name, std::vector<PropertyData>() });
			_scriptComponentData[name].reserve(fieldCount);
			for (int j = 0; j < fieldCount; j++) {
				PropertyData d;
				const char* propName;
				type->GetProperty(j, &propName, &d.typeId, &d.isPrivate, &d.isProtected, &d.offset, &d.isReference, &d.accessMask, &d.compositeOffset, &d.isCompositeIndirect);
				d.name = propName;
				d.prettyName = Prettify(d.name);
				d.typeInfo = type;

				switch (d.typeId) {
				case asTYPEID_VOID: d.typeName = "void"; break;
				case asTYPEID_BOOL: d.typeName = "bool"; break;
				case asTYPEID_INT8: d.typeName = "int8"; break;
				case asTYPEID_INT16: d.typeName = "int16"; break;
				case asTYPEID_INT32: d.typeName = "int"; break;
				case asTYPEID_INT64: d.typeName = "long"; break;
				case asTYPEID_UINT8: d.typeName = "uint8"; break;
				case asTYPEID_UINT16: d.typeName = "uint16"; break;
				case asTYPEID_UINT32: d.typeName = "unsigned int"; break;
				case asTYPEID_UINT64: d.typeName = "unsigned long"; break;
				case asTYPEID_FLOAT: d.typeName = "float"; break;
				case asTYPEID_DOUBLE: d.typeName = "double"; break;
				case asTYPEID_OBJHANDLE: d.typeName = "ptr"; break;
				case asTYPEID_HANDLETOCONST: d.typeName = "const ptr"; break;
				case asTYPEID_MASK_OBJECT: d.typeName = "mask"; break;
				case asTYPEID_APPOBJECT: d.typeName = "native object"; break;
				case asTYPEID_SCRIPTOBJECT: d.typeName = "angel object"; break;
				case asTYPEID_TEMPLATE: d.typeName = "template"; break;
				case asTYPEID_MASK_SEQNBR: d.typeName = "mask sequence"; break;
				default: d.typeName = _scriptEngine->GetTypeInfoById(d.typeId)->GetName(); break;
				}
				_scriptComponentData[name].emplace_back(d);
				printf("\t\t\t\t\t\t  %d --> %s %s\n", j, d.typeName.c_str(), d.name.c_str());
			}
			auto ctor = type->GetFactoryByIndex(0);
			ctor->AddRef();
			_scriptComponentCtors.insert({ name, ctor });
		}

		int enumCount = _scriptModule->GetEnumCount(); // get the number of all enums
		for (int i = 0; i < enumCount; i++) { // for each enum
			asITypeInfo* type = _scriptModule->GetEnumByIndex(i);
			const char* name = type->GetName();
			int enumValueCount = type->GetEnumValueCount();
			auto& values = _enums[name];
			values.reserve(enumValueCount);
			for (int j = 0; j < enumValueCount; j++) {
				int val = -1;
				const char* enumValueName = type->GetEnumValueByIndex(j, &val);
				values.emplace_back(enumValueName, val, std::move(Prettify(enumValueName)));
			}
		}
	}
	return true;
}
