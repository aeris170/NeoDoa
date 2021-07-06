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

#include "ScriptComponent.hpp"
#include "Input.hpp"
#include "TypedefsAndConstants.hpp"
#include "Log.hpp"
#include "Scene.hpp"
#include "Mesh.hpp"
#include "Model.hpp"
#include "Shader.hpp"
#include "Module.hpp"
#include "Tag.hpp"

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
static void vec3Ctor(void* memory) { new(memory) glm::vec3(); }
static void vec3CtorXYZ(void* memory, float xyz) { new(memory) glm::vec3(xyz); }
static void vec3CtorX_Y_Z(void* memory, float x, float y, float z) { new(memory) glm::vec3(x, y, z); }
static void vec4Ctor(void* memory) { new(memory) glm::vec4(); }
static void vec4CtorXYZW(void* memory, float xy) { new(memory) glm::vec4(xy); }
static void vec4CtorX_Y_Z_W(void* memory, float x, float y, float z, float w) { new(memory) glm::vec4(x, y, z, w); }
static void quatCtor(void* memory) { new(memory) glm::quat(); }
static void quatCtorVEC(void* memory, const glm::vec3& v) { new(memory) glm::quat(v); }
static void quatCtor_XYZW(void* memory, float w, float x, float y, float z) { new(memory) glm::quat(w, x, y, z); }

static void vertexCtor(void* memory) { new(memory) Vertex();  }

static void meshCtor(void* memory) { new(memory) Mesh(std::vector<Vertex>(), std::vector<GLuint>(), std::vector<std::weak_ptr<Texture>>()); }
static void meshDtor(void* memory) { ((Mesh*)memory)->~Mesh(); }

static Model* modelCtor(const std::string& name, std::vector<Mesh> meshes) { return &*(CreateModelFromMesh(name, meshes).lock()); }

static asIScriptObject* getModule(int ID, const std::string& name) {
	auto& modules = FindActiveScene().lock()->Modules(static_cast<EntityID>(ID));
	int index = modules.IndexOf(name);
	if (index == -1) {
		std::string s;
		s.reserve(128);
		s.append(modules["Tag"].As<Tag>().Label());
		s.append("(");
		s.append(std::to_string(ID));
		s.append(")");
		s.append(" does not have ");
		s.append(name);
		s.append("!");
		GetCore()->_angel->_scriptCtx->SetException(s.c_str());
		return nullptr;
	}
	asIScriptObject* mdl = modules[index];
	mdl->AddRef();
	return mdl;
}
static void attachModule(unsigned int ID, const std::string& name) {
	FindActiveScene().lock()->_attachList[ID].emplace_back(name);
}
static void detachModule(unsigned int ID, const std::string& name) {
	FindActiveScene().lock()->_detachList[ID].emplace_back(name);
}
static bool hasModule(unsigned int ID, const std::string& type) {
	auto& modules = FindActiveScene().lock()->Modules(static_cast<EntityID>(ID));
	return modules.IndexOf(type) != -1;
}
static Shader* findShader(const std::string& name) {
	auto rv = FindShader(name);
	if (rv.has_value()) return &*(rv.value().lock());
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

// prettify a string, ie. "myVar" becomes "My Var"
static std::string beautify(const std::string& str) {
	std::string rv;
	rv.reserve(32);
	rv.replace(0, 1, 1, str[0] >= 'a' ? (char)toupper(str[0]) : str[0]); // probably incorrect usage
	int i = 1;
	int j = 1;
	char ch;
	while ((ch = str[i]) != 0) {
		if (ch >= 'A' && ch <= 'Z') {
			rv.replace(j, 1, " ");
			rv.replace(j + 1, 1, 1, ch);
			j++;
		}
		else {
			rv.replace(j, 1, 1, ch);
		}
		i++;
		j++;
	}
	return rv;
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

	RegisterGLMVector<glm::vec3>("vec3");
	r = _scriptEngine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(vec3Ctor), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f(float)", asFUNCTION(vec3CtorXYZ), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f(float, float, float)", asFUNCTION(vec3CtorX_Y_Z), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("vec3", "float x", asOFFSET(glm::vec3, x)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("vec3", "float y", asOFFSET(glm::vec3, y)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("vec3", "float z", asOFFSET(glm::vec3, z)); assert(r >= 0);

	RegisterGLMVector<glm::vec4>("vec4");
	r = _scriptEngine->RegisterObjectBehaviour("vec4", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(vec4Ctor), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectBehaviour("vec4", asBEHAVE_CONSTRUCT, "void f(float)", asFUNCTION(vec4CtorXYZW), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectBehaviour("vec4", asBEHAVE_CONSTRUCT, "void f(float, float, float, float)", asFUNCTION(vec4CtorX_Y_Z_W), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("vec4", "float x", asOFFSET(glm::vec4, x)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("vec4", "float y", asOFFSET(glm::vec4, y)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("vec4", "float z", asOFFSET(glm::vec4, z)); assert(r >= 0);
	r = _scriptEngine->RegisterObjectProperty("vec4", "float w", asOFFSET(glm::vec4, w)); assert(r >= 0);

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

	r = _scriptEngine->RegisterInterface("__module"); assert(r >= 0);
#pragma endregion
#pragma region Core Functions
	r = _scriptEngine->RegisterGlobalFunction("Shader@ FindShader(const string &in)", asFUNCTION(findShader), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("Model@ FindModel(const string &in)", asFUNCTION(findModel), asCALL_CDECL); assert(r >= 0);

	r = _scriptEngine->RegisterGlobalFunction("__module@ __get(int, const string)", asFUNCTION(getModule), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void __attach(uint, const string)", asFUNCTION(attachModule), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("void __detach(uint, const string)", asFUNCTION(detachModule), asCALL_CDECL); assert(r >= 0);
	r = _scriptEngine->RegisterGlobalFunction("bool __has(uint, const string)", asFUNCTION(hasModule), asCALL_CDECL); assert(r >= 0);
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
	_scriptLoaderDeamon.join();
	_scriptModule->Discard();
	_scriptCtx->Release();
	_scriptEngine->ShutDownAndRelease();
}

bool Angel::IsModule(asITypeInfo* type) {
	return type->DerivesFrom(_moduleBaseType);
}

bool Angel::IsDefModule(asITypeInfo* type) {
	return type->DerivesFrom(_defModuleBaseType);
}

void Angel::ExecuteModule(asIScriptObject* sys, float deltaTime) {
	_scriptCtx->Prepare(_moduleUpdates[sys->GetObjectType()->GetName()]);
	_scriptCtx->SetArgFloat(0, deltaTime);
	_scriptCtx->SetObject(sys);
	if (_scriptCtx->Execute() == asEXECUTION_EXCEPTION) {
		auto* func = _scriptCtx->GetExceptionFunction();
		DOA_LOG_FATAL("Uncaught exception '%s' in %s::%s (line %d)!", _scriptCtx->GetExceptionString(), func->GetObjectName(), func->GetName(), _scriptCtx->GetExceptionLineNumber());
	}
}

Module Angel::InstantiateModule(std::string_view moduleType, int ID) {
	if (_moduleCtors.count(moduleType.data()) == 0) {
		DOA_LOG_ERROR("No such module type!");
		throw;
	}
	_scriptCtx->Prepare(_moduleCtors[moduleType.data()]);
	_scriptCtx->Execute();
	asIScriptObject* obj = *(asIScriptObject**)_scriptCtx->GetAddressOfReturnValue();
	Module m(moduleType, obj);
	m.SetID(ID);
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
	if (_scriptBuilder.AddSectionFromMemory("Module", R"(
		class Entity {
			private int _id;

			int id {
				get const {	return _id; }
			}

			Entity() { _id = -1; }
			Entity(int id) { _id = id; }
			Entity(const Entity &in other) { this._id = other._id; }
			Entity@ opAssign(const Entity &in other) {
				this._id = other._id;
				return this;
			}
		}

		abstract class Module : __module {
			Entity entity;

			void Execute(float delta) {}

			Module@ Get(const string type) const final {
				return cast<Module>(__get(entity.id, type));
			}

			void Attach(const string type) final {
				__attach(entity.id, type);
			}

			void Detach(const string type) final {
				__detach(entity.id, type);
			}

			bool Has(const string type) const final {
				return __has(entity.id, type);
			}
		}

		abstract class DefModule : Module {}

		class Tag : DefModule {
			string label;
		}

		class Transform : DefModule {
			vec3 Translation = vec3(0, 0, 0);
			quat Rotation = quat(vec3(0, 0, 0));
			vec3 Scale = vec3(1, 1, 1);

			private Transform@ parent;
			private array<Transform@> children;

			void Adopt(Transform@ child) {
				if (@child.parent != null) {
					child.parent.Disown(child);
				}
				children.insertLast(child);
				@child.parent = @this;
			}

			void Disown(Transform@ child) {
				if(@child.parent == @this) {
					children.removeAt(children.findByRef(child));
					@child.parent = null;
				}
			}
		}

		class ModelRenderer : DefModule {
			Model@ model;
			Shader@ shader;
		}
	)") < 0) {
		// Senpai... Senpai... I... I... I fucked up senpai!!
		DOA_LOG_FATAL("Couldn't compile default scripts.");
		return false;
	}
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

	{ //reset everything!
		_moduleBaseType = _scriptModule->GetTypeInfoByDecl("Module");
		_defModuleBaseType = _scriptModule->GetTypeInfoByDecl("DefModule");
		_modules.clear();
		_moduleCtors.clear();
		_moduleUpdates.clear();
	}

	{//enumerate and store all components and systems
		int classCount = _scriptModule->GetObjectTypeCount(); // get the number of all types
		for (int i = 0; i < classCount; i++) { // for each type
			asITypeInfo* type = _scriptModule->GetObjectTypeByIndex(i);
			const char* name = type->GetName();
			if (strcmp(name, "Module") == 0) continue; // skip the actual base "Module" class
			if (strcmp(name, "DefModule") == 0) continue; // skip the actual base "DefModule" class
			if (IsModule(type)) {
				DOA_LOG_INFO("Module: %s succesfully registered", name); // print its name
				int fieldCount = type->GetPropertyCount(); // and all of its fields...
				_modules.insert({ name, std::vector<PropertyData>() });
				_modules[name].reserve(fieldCount);
				for (int j = 0; j < fieldCount; j++) {
					PropertyData d;
					const char* propName;
					type->GetProperty(j, &propName, &d.typeId, &d.isPrivate, &d.isProtected, &d.offset, &d.isReference, &d.accessMask, &d.compositeOffset, &d.isCompositeIndirect);
					d.name = propName;
					d.prettyName = beautify(d.name);
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
					_modules[name].emplace_back(d);
					_moduleCtors.insert({ name, type->GetFactoryByIndex(0) });
					_moduleUpdates.insert({  name, type->GetMethodByDecl("void Execute(float)") });
					printf("\t\t\t\t\t\t  %d --> %s %s\n", j, d.typeName.c_str(), d.name.c_str());
				}
			}
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
				values.push_back({ enumValueName, val, beautify(enumValueName) });
			}
		}
	}
	return true;
}
