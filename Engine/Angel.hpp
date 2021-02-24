#pragma once

#include "Core.hpp"

#include <atomic>
#include <thread>
#include <unordered_map>
#include <unordered_set>

#include <angelscript.h>
#include <scriptbuilder/scriptbuilder.h>

#include "PropertyData.hpp"
#include "EnumValue.hpp"
#include "Module.hpp"

struct Scene;

struct Angel {

	asIScriptEngine* _scriptEngine;
	asIScriptModule* _scriptModule;
	asIScriptContext* _scriptCtx;
	CScriptBuilder _scriptBuilder;

	std::vector<std::string> _scriptFiles;
	std::vector<std::string> _scriptFilesContent;

	asITypeInfo* _moduleBaseType{ nullptr };
	asITypeInfo* _defModuleBaseType{ nullptr };
	std::unordered_map<std::string, std::vector<PropertyData>> _modules;
	std::unordered_map<std::string, asIScriptFunction*> _moduleCtors;
	std::unordered_map<std::string, asIScriptFunction*> _moduleUpdates;
	std::unordered_map<std::string, std::vector<EnumValue>> _enums;

	std::atomic<bool> _scriptLoaderRunning{ true };
	std::thread _scriptLoaderDeamon;

	Angel() noexcept;
	~Angel() noexcept;

	bool IsModule(asITypeInfo* type);
	bool IsDefModule(asITypeInfo* type);

	void ExecuteModule(asIScriptObject* sys, float deltaTime);
	Module InstantiateModule(std::string_view moduleType, int ID);
private:
	Angel(const Angel&) = delete;
	Angel(const Angel&&) = delete;
	Angel& operator=(const Angel&) = delete;
	Angel& operator=(const Angel&&) = delete;

	void FindAndBuildScripts();
	bool Rebuild();

	template <typename T>
	void RegisterGLMVector(const char* typeName) {
		std::string t{ typeName };
		static int r;
		r = _scriptEngine->RegisterObjectType(typeName, sizeof(T), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_ALLFLOATS | asGetTypeTraits<T>()); assert(r >= 0);
		r = _scriptEngine->RegisterObjectMethod(typeName, std::string(t + "& opAddAssign(const " + t + " &in) const").c_str(), asMETHODPR(T, operator+=, (const T&), T&), asCALL_THISCALL); assert(r >= 0);
		r = _scriptEngine->RegisterObjectMethod(typeName, std::string(t + "& opSubAssign(const " + t + " &in) const").c_str(), asMETHODPR(T, operator-=, (const T&), T&), asCALL_THISCALL); assert(r >= 0);
		r = _scriptEngine->RegisterObjectMethod(typeName, std::string(t + "& opMulAssign(const " + t + " &in) const").c_str(), asMETHODPR(T, operator*=, (const T&), T&), asCALL_THISCALL); assert(r >= 0);
		r = _scriptEngine->RegisterObjectMethod(typeName, std::string(t + "& opDivAssign(const " + t + " &in) const").c_str(), asMETHODPR(T, operator/=, (const T&), T&), asCALL_THISCALL); assert(r >= 0);
		r = _scriptEngine->RegisterObjectMethod(typeName, std::string(t + "& opAssign(const " + t + " &in) const").c_str(), asMETHODPR(T, operator=, (const T&), T&), asCALL_THISCALL); assert(r >= 0);
		r = _scriptEngine->RegisterObjectMethod(typeName, std::string(t + "& opAdd(const " + t + "& in) const").c_str(), asFUNCTIONPR(glm::operator+, (const T&, const T&), T), asCALL_CDECL_OBJLAST); assert(r >= 0);
		r = _scriptEngine->RegisterObjectMethod(typeName, std::string(t + "& opSub(const " + t + "& in) const").c_str(), asFUNCTIONPR(glm::operator-, (const T&, const T&), T), asCALL_CDECL_OBJLAST); assert(r >= 0);
		r = _scriptEngine->RegisterObjectMethod(typeName, std::string(t + "& opMul(const " + t + "& in) const").c_str(), asFUNCTIONPR(glm::operator*, (const T&, const T&), T), asCALL_CDECL_OBJLAST); assert(r >= 0);
		r = _scriptEngine->RegisterObjectMethod(typeName, std::string(t + "& opDiv(const " + t + "& in) const").c_str(), asFUNCTIONPR(glm::operator/, (const T&, const T&), T), asCALL_CDECL_OBJLAST); assert(r >= 0);
	}
};
