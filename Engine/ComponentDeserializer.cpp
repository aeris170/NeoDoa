#include "ComponentDeserializer.hpp"

#include <regex>
#include <vector>
#include <sstream>

#include <Utility/split.hpp>

#include "Core.hpp"
#include "Angel.hpp"
#include "Log.hpp"

#include "FileNode.hpp"

#ifdef NO_ANGEL_SCRIPT

static struct ComponentDecl {
    const std::string fullDeclaration;
    const std::string name;
    const std::string fieldDeclarations;
};

static struct FieldDecl {
    std::string typeName;
    std::string name;
    std::string value;
};

static std::string remove_indentation(std::string source);
static std::string remove_comments(std::string source);
static std::string remove_new_lines(std::string source);
static ComponentDecl find_component_decl(std::string source);
static std::vector<FieldDecl> extract_field_decl(std::string fieldsSource);

Component DeserializeComponent(const std::string_view source) {
    Component rv;
    rv.declaration = source;

    std::string sourceCode{ source };
    sourceCode = remove_indentation(sourceCode);
    sourceCode = remove_comments(sourceCode);
    ComponentDecl decl = find_component_decl(sourceCode);

    std::vector<std::string> lines = split(decl.fieldDeclarations, ";");
    lines.erase(std::remove_if(lines.begin(), lines.end(), [](auto elem) { return elem.length() == 0; }), lines.end());
    for (auto& line : lines) {
        auto fields = extract_field_decl(line);
        for (auto& field : fields) {
            rv.fields.emplace_back(field.typeName, field.name, field.value);
        }
    }
    rv.name = decl.name;

    return rv;
}
Component DeserializeComponent(const FNode& file) {
    file.ReadContent();
    return DeserializeComponent(file.DisposeContent());
}

static std::string remove_indentation(std::string source) {
    return std::regex_replace(source, std::regex("\n[ \t]*"), "\n");
}

static std::string remove_comments(std::string source) {
    source = std::regex_replace(source, std::regex("[ \t]*\\/\\/.*\n"), "\n");
    source = std::regex_replace(source, std::regex("[ \t]*\\/\\*+[\\s\\S]*?\\*+\\/"), "");
    return source;
}

static std::string remove_new_lines(std::string source) {
    return std::regex_replace(source, std::regex("\n*"), "");
}

static ComponentDecl find_component_decl(std::string source) {
    static std::regex regexp{ "component +([a-zA-Z_]*[a-zA-Z0-9_]*) *\\{([\\s\\S]*?)\\}[\\s]*;" };

    std::smatch match;
    std::regex_search(source, match, regexp);
    return { match[0], match[1], remove_new_lines(match[2]) };
}

static std::vector<FieldDecl> extract_field_decl(std::string fieldsSource) {
    static std::regex regexp{ "(\\w*)\\s+(\\w*)(\\s*\\{\\s*([\\s\\S]*?)\\s*\\})?" };

    std::vector<FieldDecl> rv;
    std::smatch match;
    std::regex_search(fieldsSource, match, regexp);
    FieldDecl decl;
    decl.typeName = match[1];
    decl.name = match[2];
    if (match[4] == true) {
        decl.value = match[4];
    }
    rv.emplace_back(std::move(decl));
    return rv;
}

#endif

static void CompilationMessageCallback(const asSMessageInfo* msg, void* param) {
    ComponentDeserializationResult& cdr = *reinterpret_cast<ComponentDeserializationResult*>(param);

    ComponentCompilerMessage m;
    if (msg->type == asMSGTYPE_INFORMATION) {
        m.messageType = ComponentCompilerMessageType::INFO;
        DOA_LOG_INFO("%s (%d, %d) : %s", msg->section, msg->row, msg->col, msg->message);
    } else if (msg->type == asMSGTYPE_WARNING) {
        m.messageType = ComponentCompilerMessageType::WARNING;
        DOA_LOG_WARNING("%s (%d, %d) : %s", msg->section, msg->row, msg->col, msg->message);
    } else {
        cdr.erred = true;
        m.messageType = ComponentCompilerMessageType::ERROR;
        DOA_LOG_ERROR("%s (%d, %d) : %s", msg->section, msg->row, msg->col, msg->message);
    }
    m.message = msg->message;
    m.lineNo = msg->row;
    m.columnNo = msg->col;

    cdr.messages.emplace_back(std::move(m));
}

ComponentDeserializationResult DeserializeComponent(const FNode& file) {
    static const auto& angel{ Core::GetCore()->Angel() };
    static auto& scriptEngine = angel->ScriptEngine();
    static auto& scriptBuilder = angel->ScriptBuilder();
    ComponentDeserializationResult rv{};
    {
        int r = scriptEngine.SetMessageCallback(asFUNCTION(CompilationMessageCallback), &rv, asCALL_CDECL); assert(r >= 0);
    }

    file.ReadContent();
    std::string content{ file.DisposeContent() };

    std::string fileUID = file.AbsolutePath().string();
    if (scriptBuilder.StartNewModule(&scriptEngine, fileUID.c_str()) < 0) {
        // If the code fails here it is usually because there
        // is no more memory to allocate the module, buy more ram maybeeeeee
        static std::string errmsg{ "Component deserialization failed! Unrecoverable error while starting a new module. Most probable cause is insufficient memory." };
        DOA_LOG_ERROR(errmsg.c_str());
        rv.messages.emplace_back(
            1, 1,
            ComponentCompilerMessageType::ERROR,
            errmsg
        );
        rv.erred = true;
        return rv;
    }
    auto scriptModule = scriptEngine.GetModule(fileUID.c_str());

    std::string fileName = file.FullName();
    if (scriptBuilder.AddSectionFromMemory(fileUID.c_str(), content.c_str()) < 0) {
        // The builder wasn't able to load the file. Maybe the file
        // has been removed, or the wrong name was given, or some
        // preprocessing commands are incorrectly written.
        static std::string errmsg{ std::format("Component deserialization failed! Please correct the errors in {} and try deserializing again.", fileName) };
        DOA_LOG_ERROR(errmsg.c_str(), fileName.c_str());
        rv.messages.emplace_back(
            1, 1,
            ComponentCompilerMessageType::ERROR,
            errmsg
        );
        rv.erred = true;
        return rv;
    }

    if (scriptBuilder.BuildModule() < 0) {
        // An error occurred. Instruct the script writer to fix the
        // compilation errors that were listed in the output stream.
        static std::string errmsg{ std::format("Component deserialization failed! Please correct the errors in {} and try deserializing again.", fileName) };
        DOA_LOG_ERROR(errmsg.c_str());
        rv.messages.emplace_back(
            1, 1,
            ComponentCompilerMessageType::ERROR,
            errmsg
        );
        rv.erred = true;
        return rv;
    }

    auto declaredObjectCount = scriptModule->GetObjectTypeCount();
    if (declaredObjectCount == 0) {
        static std::string errmsg{ std::format("Component deserialization failed! Couldn't find a component definition in {}.", fileName) };
        DOA_LOG_ERROR(errmsg.c_str(), fileName.c_str());
        rv.messages.emplace_back(
            1, 1,
            ComponentCompilerMessageType::ERROR,
            errmsg
        );
        rv.erred = true;
        return rv;
    }

    { // Check component count - we can have one and only one component per file!
        int componentDefinitionCount{ 0 };
        for (auto i = 0; i < declaredObjectCount; i++) {
            auto typeInfo = scriptModule->GetObjectTypeByIndex(i);
            if (angel->IsComponentDefinition(typeInfo)) {
                componentDefinitionCount++;
            }
        }

        if (componentDefinitionCount == 0) {
            static std::string errmsg{ std::format("Component deserialization failed! Couldn't find a component definition in {}.", fileName) };
            DOA_LOG_ERROR(errmsg.c_str(), fileName.c_str());
            rv.messages.emplace_back(
                1, 1,
                ComponentCompilerMessageType::ERROR,
                errmsg
            );
            rv.erred = true;
            return rv;
        }

        if (componentDefinitionCount > 1) {
            static std::string warnmsg{ std::format("Component deserialization warning! Found multiple components in {}. Only the first definition can be used, others are stripped from binary.", fileName) };
            DOA_LOG_WARNING(warnmsg.c_str(), fileName.c_str());
            rv.messages.emplace_back(
                1, 1,
                ComponentCompilerMessageType::WARNING,
                warnmsg
            );
        }
    }


    for (auto i = 0; i < declaredObjectCount; i++) {
        auto typeInfo = scriptModule->GetObjectTypeByIndex(i);
        if (angel->IsComponentDefinition(typeInfo)) {
            if (typeInfo->GetFactoryCount() > 0) {
                rv.messages.emplace_back(
                    1, 1,
                    ComponentCompilerMessageType::INFO,
                    "Constructors are stripped from binary, do not use constructors!"
                );
            }

            rv.deserializedComponent.name = typeInfo->GetName();
            rv.deserializedComponent.declaration = content;

            int fieldCount = typeInfo->GetPropertyCount();
            for (int j = 0; j < fieldCount; j++) {
                PropertyData d;
                const char* propName;
                typeInfo->GetProperty(j, &propName, &d.typeId, &d.isPrivate, &d.isProtected, &d.offset, &d.isReference, &d.accessMask, &d.compositeOffset, &d.isCompositeIndirect);
                d.name = propName;
                d.typeInfo = typeInfo;

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
                    default: d.typeName = scriptEngine.GetTypeInfoById(d.typeId)->GetName(); break;
                }

                rv.deserializedComponent.fields.emplace_back(d.typeName, d.name);
            }
            break;
        }
    }

    return rv;
}

ComponentDeserializationResult DeserializeComponent(const std::string_view data) {
    static const auto& angel{ Core::GetCore()->Angel() };
    static auto& scriptEngine = angel->ScriptEngine();
    static auto& scriptBuilder = angel->ScriptBuilder();

    std::string content{ data };

    if (scriptBuilder.StartNewModule(&scriptEngine, content.c_str()) < 0) {
        // If the code fails here it is usually because there
        // is no more memory to allocate the module, buy more ram maybeeeeee
        DOA_LOG_ERROR("Component deserialization failed! Unrecoverable error while starting a new module. Most probable cause is insufficient memory.");
        return ComponentDeserializationResult{ .erred{ true } };
    }
    auto scriptModule = scriptEngine.GetModule(content.c_str());

    if (scriptBuilder.AddSectionFromMemory(content.c_str(), content.c_str()) < 0) {
        // The builder wasn't able to load the file. Maybe the file
        // has been removed, or the wrong name was given, or some
        // preprocessing commands are incorrectly written.
        DOA_LOG_ERROR("Component deserialization failed! Please correct the errors and try deserializing again.");
        return ComponentDeserializationResult{ .erred{ true } };
    }

    if (scriptBuilder.BuildModule() < 0) {
        // An error occurred. Instruct the script writer to fix the
        // compilation errors that were listed in the output stream.
        DOA_LOG_ERROR("Component deserialization failed! Please correct the errors and try deserializing again.");
        return ComponentDeserializationResult{ .erred{ true } };
    }

    ComponentDeserializationResult rv;

    auto declaredObjectCount = scriptModule->GetObjectTypeCount();
    if (declaredObjectCount == 0) {
        DOA_LOG_ERROR("Component deserialization failed! Couldn't find a component definition!");
        return ComponentDeserializationResult{ .erred{ true } };
    }
    for (auto i = 0; i < declaredObjectCount; i++) {
        auto typeInfo = scriptModule->GetObjectTypeByIndex(i);
        if (!angel->IsComponentDefinition(typeInfo)) {
            rv.deserializedComponent.name = typeInfo->GetName();

            int fieldCount = typeInfo->GetPropertyCount();
            for (int j = 0; j < fieldCount; j++) {
                PropertyData d;
                const char* propName;
                typeInfo->GetProperty(j, &propName, &d.typeId, &d.isPrivate, &d.isProtected, &d.offset, &d.isReference, &d.accessMask, &d.compositeOffset, &d.isCompositeIndirect);
                d.name = propName;
                d.typeInfo = typeInfo;

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
                default: d.typeName = scriptEngine.GetTypeInfoById(d.typeId)->GetName(); break;
                }

                rv.deserializedComponent.fields.emplace_back(d.typeName, d.name);
            }
        }
    }

    rv.deserializedComponent.declaration = content;
    return rv;
}