#include "Angel.hpp"

#include <angelscript.h>
#include <angelscript/scriptstdstring/scriptstdstring.h>
#include <angelscript/scriptany/scriptany.h>
#include <angelscript/scriptarray/scriptarray.h>
#include <angelscript/scriptdictionary/scriptdictionary.h>
#include <angelscript/scriptfile/scriptfile.h>
#include <angelscript/scriptgrid/scriptgrid.h>
#include <angelscript/scripthandle/scripthandle.h>
#include <angelscript/scripthelper/scripthelper.h>
#include <angelscript/scriptmath/scriptmath.h>
#include <angelscript/datetime/datetime.h>

#include <Engine/Log.hpp>

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

    int r;
    r = _scriptEngine->SetMessageCallback(asFUNCTION(MessageCallback), nullptr, asCALL_CDECL); assert(r >= 0);
    r = _scriptEngine->RegisterInterface("Component"); assert(r >= 0);
    _componentTypeInfo = _scriptEngine->GetTypeInfoByName("Component");

    _scriptCtx = _scriptEngine->CreateContext();
}

Angel::~Angel() noexcept {
    _scriptCtx->Release();
    _scriptEngine->ShutDownAndRelease();
}

asIScriptEngine& Angel::ScriptEngine() { return *_scriptEngine; }
CScriptBuilder& Angel::ScriptBuilder() { return _scriptBuilder; }
bool Angel::IsComponentDefinition(asITypeInfo* typeInfo) const { return typeInfo->Implements(_componentTypeInfo); }
