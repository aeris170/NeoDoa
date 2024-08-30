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

    asITypeInfo* _componentTypeInfo;
};
