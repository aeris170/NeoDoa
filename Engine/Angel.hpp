#pragma once

#include <cassert>

#include <angelscript.h>
#include <angelscript/scriptbuilder/scriptbuilder.h>

#include <glm/glm.hpp>

#include "PropertyData.hpp"

struct Angel {
    Angel() noexcept;
    ~Angel() noexcept;

    asIScriptEngine& ScriptEngine();
    CScriptBuilder& ScriptBuilder();

    bool IsComponentDefinition(asITypeInfo* typeInfo) const;

private:
    asIScriptEngine* _scriptEngine;
    asIScriptModule* _scriptModule;
    asIScriptContext* _scriptCtx;
    CScriptBuilder _scriptBuilder;

    Angel(const Angel&) = delete;
    Angel(const Angel&&) = delete;
    Angel& operator=(const Angel&) = delete;
    Angel& operator=(const Angel&&) = delete;

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

    asITypeInfo* _componentTypeInfo;
};
