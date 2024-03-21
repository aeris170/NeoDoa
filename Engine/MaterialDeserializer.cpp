#include <Engine/MaterialDeserializer.hpp>

#include <format>
#include <cassert>

#include <tinyxml2.h>

#include <Utility/NameOf.hpp>

#include <Engine/Log.hpp>
#include <Engine/Core.hpp>
#include <Engine/FileNode.hpp>

MaterialDeserializationResult DeserializeMaterial(const FNode& file) noexcept {
    file.ReadContent();
    auto rv = DeserializeMaterial(file.DisposeContent());
    if (!rv.erred) {
        rv.deserializedMaterial.Name = file.Name();
    }
    return rv;
}

MaterialDeserializationResult DeserializeMaterial(const std::string_view data) noexcept {
    MaterialDeserializationResult rv;

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError err = doc.Parse(data.data());
    if (err != tinyxml2::XML_SUCCESS) {
        rv.erred = true;
        rv.errors.emplace_back("Couldn't deserialize material!");
        rv.errors.emplace_back("This should normally never happen. If you didn't try to edit the file manually, please submit an issue.");
        DOA_LOG_ERROR("Couldn't deserialize material!\n\n%s", data);
    } else {
        MaterialDeserializer::Deserialize(*doc.RootElement(), rv);
    }

    return rv;
}

void MaterialDeserializer::DefaultDeserialize(tinyxml2::XMLElement& rootElem, MaterialDeserializationResult& mdr) noexcept {
    Program::Deserialize(*rootElem.FirstChildElement("program"), mdr);
    Uniforms::Deserialize(*rootElem.FirstChildElement("uniforms"), mdr);
}

void MaterialDeserializer::Program::DefaultDeserialize(tinyxml2::XMLElement& programElem, MaterialDeserializationResult& mdr) noexcept {
    DeserializeID(programElem, mdr);
}
void MaterialDeserializer::Program::DefaultDeserializeID(tinyxml2::XMLElement& programElem, MaterialDeserializationResult& mdr) noexcept {
    static auto& Core = Core::GetCore();

    UUID programID = programElem.Unsigned64Attribute("id", UUID::Empty());
    AssetHandle handle = Core->GetAssets()->FindAsset(programID);
    if (handle.HasValue()) {
        mdr.deserializedMaterial.ShaderProgram = handle->ID();
    } else {
        mdr.erred = true;
        mdr.errors.emplace_back(std::format("Material references non-existant program (ID:{})", programID.AsString()));
    }
}

void MaterialDeserializer::Uniforms::DefaultDeserialize(tinyxml2::XMLElement& uniformElem, MaterialDeserializationResult& mdr) noexcept {
    DeserializeVertexUniforms(*uniformElem.FirstChildElement("vertex-uniforms"), mdr);
    DeserializeTessellationControlUniforms(*uniformElem.FirstChildElement("tessellation-control-uniforms"), mdr);
    DeserializeTessellationEvaluationUniforms(*uniformElem.FirstChildElement("tessellation-evaluation-uniforms"), mdr);
    DeserializeGeometryUniforms(*uniformElem.FirstChildElement("geometry-uniforms"), mdr);
    DeserializeFragmentUniforms(*uniformElem.FirstChildElement("fragment-uniforms"), mdr);
}
void MaterialDeserializer::Uniforms::DefaultDeserializeVertexUniforms(tinyxml2::XMLElement& vertexUniformsElem, MaterialDeserializationResult& mdr) noexcept {
    Material::Uniforms uniforms;
    for (tinyxml2::XMLElement* uniform = vertexUniformsElem.FirstChildElement(); uniform != nullptr; uniform = uniform->NextSiblingElement()) {
        std::string name = uniform->Name();
        if (name == "uniform") {
            DeserializeUniform(*uniform, mdr, uniforms);
        } else {
            mdr.erred = true;
            mdr.errors.emplace_back(std::format("Error while deserializing material, expected (uniform | uniform-vector) - received {}", name));
        }
    }
    mdr.deserializedMaterial.VertexUniforms = std::move(uniforms);
}
void MaterialDeserializer::Uniforms::DefaultDeserializeTessellationControlUniforms(tinyxml2::XMLElement& tessellationControlUniformsElem, MaterialDeserializationResult& mdr) noexcept {
    Material::Uniforms uniforms;
    for (tinyxml2::XMLElement* uniform = tessellationControlUniformsElem.FirstChildElement(); uniform != nullptr; uniform = uniform->NextSiblingElement()) {
        std::string name = uniform->Name();
        if (name == "uniform") {
            DeserializeUniform(*uniform, mdr, uniforms);
        } else {
            mdr.erred = true;
            mdr.errors.emplace_back(std::format("Error while deserializing material, expected (uniform | uniform-vector) - received {}", name));
        }
    }
    mdr.deserializedMaterial.TessellationControlUniforms = std::move(uniforms);
}
void MaterialDeserializer::Uniforms::DefaultDeserializeTessellationEvaluationUniforms(tinyxml2::XMLElement& tessellationEvaluationUniformsElem, MaterialDeserializationResult& mdr) noexcept {
    Material::Uniforms uniforms;
    for (tinyxml2::XMLElement* uniform = tessellationEvaluationUniformsElem.FirstChildElement(); uniform != nullptr; uniform = uniform->NextSiblingElement()) {
        std::string name = uniform->Name();
        if (name == "uniform") {
            DeserializeUniform(*uniform, mdr, uniforms);
        } else {
            mdr.erred = true;
            mdr.errors.emplace_back(std::format("Error while deserializing material, expected (uniform | uniform-vector) - received {}", name));
        }
    }
    mdr.deserializedMaterial.TessellationEvaluationUniforms = std::move(uniforms);
}
void MaterialDeserializer::Uniforms::DefaultDeserializeGeometryUniforms(tinyxml2::XMLElement& geometryUniformsElem, MaterialDeserializationResult& mdr) noexcept {
    Material::Uniforms uniforms;
    for (tinyxml2::XMLElement* uniform = geometryUniformsElem.FirstChildElement(); uniform != nullptr; uniform = uniform->NextSiblingElement()) {
        std::string name = uniform->Name();
        if (name == "uniform") {
            DeserializeUniform(*uniform, mdr, uniforms);
        } else {
            mdr.erred = true;
            mdr.errors.emplace_back(std::format("Error while deserializing material, expected (uniform | uniform-vector) - received {}", name));
        }
    }
    mdr.deserializedMaterial.GeometryUniforms = std::move(uniforms);
}
void MaterialDeserializer::Uniforms::DefaultDeserializeFragmentUniforms(tinyxml2::XMLElement& fragmentUniformsElem, MaterialDeserializationResult& mdr) noexcept {
    Material::Uniforms uniforms;
    for (tinyxml2::XMLElement* uniform = fragmentUniformsElem.FirstChildElement(); uniform != nullptr; uniform = uniform->NextSiblingElement()) {
        std::string name = uniform->Name();
        if (name == "uniform") {
            DeserializeUniform(*uniform, mdr, uniforms);
        } else {
            mdr.erred = true;
            mdr.errors.emplace_back(std::format("Error while deserializing material, expected (uniform | uniform-vector) - received {}", name));
        }
    }
    mdr.deserializedMaterial.FragmentUniforms = std::move(uniforms);
}
void MaterialDeserializer::Uniforms::DefaultDeserializeUniform(tinyxml2::XMLElement& uniformElem, MaterialDeserializationResult& mdr, Material::Uniforms& uniformsToFill) noexcept {
    int location = uniformElem.IntAttribute("location", UniformValue::InvalidLocation);
    std::string_view name = uniformElem.Attribute("name");
    std::string_view typeString = uniformElem.Attribute("type");
    Helpers::UniformType type = Helpers::ExtractUniformType(typeString);

    // then we iterate each "value" node, deserialize and emplace the deserialized content into our vector
    const tinyxml2::XMLElement* valueElemPtr = uniformElem.FirstChildElement("value");
    if (!valueElemPtr) {
        mdr.erred = true;
        mdr.errors.emplace_back(std::format("Error while deserializing material, uniform location {} has no value!", location));
        return;
    }
    const tinyxml2::XMLElement& valueElem = *valueElemPtr;

    switch (type) {
    case MaterialDeserializer::Helpers::UniformType::Uniform1f:
        uniformsToFill.Set(location, name, Helpers::DeserializeUniform1f(valueElem));
        break;
    case MaterialDeserializer::Helpers::UniformType::Uniform2f:
        uniformsToFill.Set(location, name, Helpers::DeserializeUniform2f(valueElem));
        break;
    case MaterialDeserializer::Helpers::UniformType::Uniform3f:
        uniformsToFill.Set(location, name, Helpers::DeserializeUniform3f(valueElem));
        break;
    case MaterialDeserializer::Helpers::UniformType::Uniform4f:
        uniformsToFill.Set(location, name, Helpers::DeserializeUniform4f(valueElem));
        break;
    case MaterialDeserializer::Helpers::UniformType::Uniform1i:
        uniformsToFill.Set(location, name, Helpers::DeserializeUniform1i(valueElem));
        break;
    case MaterialDeserializer::Helpers::UniformType::Uniform2i:
        uniformsToFill.Set(location, name, Helpers::DeserializeUniform2i(valueElem));
        break;
    case MaterialDeserializer::Helpers::UniformType::Uniform3i:
        uniformsToFill.Set(location, name, Helpers::DeserializeUniform3i(valueElem));
        break;
    case MaterialDeserializer::Helpers::UniformType::Uniform4i:
        uniformsToFill.Set(location, name, Helpers::DeserializeUniform4i(valueElem));
        break;
    case MaterialDeserializer::Helpers::UniformType::Uniform1ui:
        uniformsToFill.Set(location, name, Helpers::DeserializeUniform1ui(valueElem));
        break;
    case MaterialDeserializer::Helpers::UniformType::Uniform2ui:
        uniformsToFill.Set(location, name, Helpers::DeserializeUniform2ui(valueElem));
        break;
    case MaterialDeserializer::Helpers::UniformType::Uniform3ui:
        uniformsToFill.Set(location, name, Helpers::DeserializeUniform3ui(valueElem));
        break;
    case MaterialDeserializer::Helpers::UniformType::Uniform4ui:
        uniformsToFill.Set(location, name, Helpers::DeserializeUniform4ui(valueElem));
        break;

    case MaterialDeserializer::Helpers::UniformType::UniformMatrix2f:
        uniformsToFill.Set(location, name, Helpers::DeserializeUniformMatrix2f(valueElem));
        break;
    case MaterialDeserializer::Helpers::UniformType::UniformMatrix3f:
        uniformsToFill.Set(location, name, Helpers::DeserializeUniformMatrix3f(valueElem));
        break;
    case MaterialDeserializer::Helpers::UniformType::UniformMatrix4f:
        uniformsToFill.Set(location, name, Helpers::DeserializeUniformMatrix4f(valueElem));
        break;
    case MaterialDeserializer::Helpers::UniformType::UniformMatrix2x3f:
        uniformsToFill.Set(location, name, Helpers::DeserializeUniformMatrix2x3f(valueElem));
        break;
    case MaterialDeserializer::Helpers::UniformType::UniformMatrix3x2f:
        uniformsToFill.Set(location, name, Helpers::DeserializeUniformMatrix3x2f(valueElem));
        break;
    case MaterialDeserializer::Helpers::UniformType::UniformMatrix2x4f:
        uniformsToFill.Set(location, name, Helpers::DeserializeUniformMatrix2x4f(valueElem));
        break;
    case MaterialDeserializer::Helpers::UniformType::UniformMatrix4x2f:
        uniformsToFill.Set(location, name, Helpers::DeserializeUniformMatrix4x2f(valueElem));
        break;
    case MaterialDeserializer::Helpers::UniformType::UniformMatrix3x4f:
        uniformsToFill.Set(location, name, Helpers::DeserializeUniformMatrix3x4f(valueElem));
        break;
    case MaterialDeserializer::Helpers::UniformType::UniformMatrix4x3f:
        uniformsToFill.Set(location, name, Helpers::DeserializeUniformMatrix4x3f(valueElem));
        break;
    default:
        mdr.erred = true;
        mdr.errors.emplace_back(std::format("Error while deserializing material, uniform location {} has invalid enum!", location));
        break;
    }
}

MaterialDeserializer::Helpers::UniformType MaterialDeserializer::Helpers::ExtractUniformType(const std::string_view typeString) noexcept {
    if      (typeString == nameof(Uniform1f)) { return UniformType::Uniform1f; }
    else if (typeString == nameof(Uniform2f)) { return UniformType::Uniform2f; }
    else if (typeString == nameof(Uniform3f)) { return UniformType::Uniform3f; }
    else if (typeString == nameof(Uniform4f)) { return UniformType::Uniform4f; }

    else if (typeString == nameof(Uniform1i)) { return UniformType::Uniform1i; }
    else if (typeString == nameof(Uniform2i)) { return UniformType::Uniform2i; }
    else if (typeString == nameof(Uniform3i)) { return UniformType::Uniform3i; }
    else if (typeString == nameof(Uniform4i)) { return UniformType::Uniform4i; }

    else if (typeString == nameof(Uniform1ui)) { return UniformType::Uniform1ui; }
    else if (typeString == nameof(Uniform2ui)) { return UniformType::Uniform2ui; }
    else if (typeString == nameof(Uniform3ui)) { return UniformType::Uniform3ui; }
    else if (typeString == nameof(Uniform4ui)) { return UniformType::Uniform4ui; }

    else if (typeString == nameof(UniformMatrix2f))   { return UniformType::UniformMatrix2f;   }
    else if (typeString == nameof(UniformMatrix3f))   { return UniformType::UniformMatrix3f;   }
    else if (typeString == nameof(UniformMatrix4f))   { return UniformType::UniformMatrix4f;   }
    else if (typeString == nameof(UniformMatrix2x3f)) { return UniformType::UniformMatrix2x3f; }
    else if (typeString == nameof(UniformMatrix3x2f)) { return UniformType::UniformMatrix3x2f; }
    else if (typeString == nameof(UniformMatrix2x4f)) { return UniformType::UniformMatrix2x4f; }
    else if (typeString == nameof(UniformMatrix4x2f)) { return UniformType::UniformMatrix4x2f; }
    else if (typeString == nameof(UniformMatrix3x4f)) { return UniformType::UniformMatrix3x4f; }
    else if (typeString == nameof(UniformMatrix4x3f)) { return UniformType::UniformMatrix4x3f; }
    else {
        // invalid enum
        DOA_LOG_ERROR("Couldn't extract uniform type. Data is corrupted.");
        assert(false);
        return UniformType::InvalidEnum;
    }
}

Uniform1f MaterialDeserializer::Helpers::DeserializeUniform1f(const tinyxml2::XMLElement& elem) noexcept {
    return Uniform1f {
        elem.FloatAttribute("index0", 0)
    };
}
Uniform2f MaterialDeserializer::Helpers::DeserializeUniform2f(const tinyxml2::XMLElement& elem) noexcept {
    return {
        elem.FloatAttribute("index0", 0),
        elem.FloatAttribute("index1", 0),
    };
}
Uniform3f MaterialDeserializer::Helpers::DeserializeUniform3f(const tinyxml2::XMLElement& elem) noexcept {
    return {
        elem.FloatAttribute("index0", 0),
        elem.FloatAttribute("index1", 0),
        elem.FloatAttribute("index2", 0),
    };
}
Uniform4f MaterialDeserializer::Helpers::DeserializeUniform4f(const tinyxml2::XMLElement& elem) noexcept {
    return {
        elem.FloatAttribute("index0", 0),
        elem.FloatAttribute("index1", 0),
        elem.FloatAttribute("index2", 0),
        elem.FloatAttribute("index3", 0),
    };
}

Uniform1i MaterialDeserializer::Helpers::DeserializeUniform1i(const tinyxml2::XMLElement& elem) noexcept {
    return Uniform1i {
        elem.IntAttribute("index0", 0)
    };
}
Uniform2i MaterialDeserializer::Helpers::DeserializeUniform2i(const tinyxml2::XMLElement& elem) noexcept {
    return {
        elem.IntAttribute("index0", 0),
        elem.IntAttribute("index1", 0),
    };
}
Uniform3i MaterialDeserializer::Helpers::DeserializeUniform3i(const tinyxml2::XMLElement& elem) noexcept {
    return {
        elem.IntAttribute("index0", 0),
        elem.IntAttribute("index1", 0),
        elem.IntAttribute("index2", 0),
    };
}
Uniform4i MaterialDeserializer::Helpers::DeserializeUniform4i(const tinyxml2::XMLElement& elem) noexcept {
    return {
        elem.IntAttribute("index0", 0),
        elem.IntAttribute("index1", 0),
        elem.IntAttribute("index2", 0),
        elem.IntAttribute("index3", 0),
    };
}

Uniform1ui MaterialDeserializer::Helpers::DeserializeUniform1ui(const tinyxml2::XMLElement& elem) noexcept {
    return Uniform1ui {
        elem.UnsignedAttribute("index0", 0)
    };
}
Uniform2ui MaterialDeserializer::Helpers::DeserializeUniform2ui(const tinyxml2::XMLElement& elem) noexcept {
    return {
        elem.UnsignedAttribute("index0", 0),
        elem.UnsignedAttribute("index1", 0),
    };
}
Uniform3ui MaterialDeserializer::Helpers::DeserializeUniform3ui(const tinyxml2::XMLElement& elem) noexcept {
    return {
        elem.UnsignedAttribute("index0", 0),
        elem.UnsignedAttribute("index1", 0),
        elem.UnsignedAttribute("index2", 0),
    };
}
Uniform4ui MaterialDeserializer::Helpers::DeserializeUniform4ui(const tinyxml2::XMLElement& elem) noexcept {
    return {
        elem.UnsignedAttribute("index0", 0),
        elem.UnsignedAttribute("index1", 0),
        elem.UnsignedAttribute("index2", 0),
        elem.UnsignedAttribute("index3", 0),
    };
}

UniformMatrix2f MaterialDeserializer::Helpers::DeserializeUniformMatrix2f(const tinyxml2::XMLElement& elem) noexcept {
    return {
        elem.FloatAttribute("index00", 0), elem.FloatAttribute("index01", 0),
        elem.FloatAttribute("index10", 0), elem.FloatAttribute("index11", 0),
    };
}
UniformMatrix3f MaterialDeserializer::Helpers::DeserializeUniformMatrix3f(const tinyxml2::XMLElement& elem) noexcept {
    return {
        elem.FloatAttribute("index00", 0), elem.FloatAttribute("index01", 0), elem.FloatAttribute("index02", 0),
        elem.FloatAttribute("index10", 0), elem.FloatAttribute("index11", 0), elem.FloatAttribute("index12", 0),
        elem.FloatAttribute("index20", 0), elem.FloatAttribute("index21", 0), elem.FloatAttribute("index22", 0),
    };
}
UniformMatrix4f MaterialDeserializer::Helpers::DeserializeUniformMatrix4f(const tinyxml2::XMLElement& elem) noexcept {
    return {
        elem.FloatAttribute("index00", 0), elem.FloatAttribute("index01", 0), elem.FloatAttribute("index02", 0), elem.FloatAttribute("index03", 0),
        elem.FloatAttribute("index10", 0), elem.FloatAttribute("index11", 0), elem.FloatAttribute("index12", 0), elem.FloatAttribute("index13", 0),
        elem.FloatAttribute("index20", 0), elem.FloatAttribute("index21", 0), elem.FloatAttribute("index22", 0), elem.FloatAttribute("index23", 0),
        elem.FloatAttribute("index30", 0), elem.FloatAttribute("index31", 0), elem.FloatAttribute("index32", 0), elem.FloatAttribute("index33", 0)
    };
}
UniformMatrix2x3f MaterialDeserializer::Helpers::DeserializeUniformMatrix2x3f(const tinyxml2::XMLElement& elem) noexcept {
    return {
        elem.FloatAttribute("index00", 0), elem.FloatAttribute("index01", 0),
        elem.FloatAttribute("index10", 0), elem.FloatAttribute("index11", 0),
        elem.FloatAttribute("index20", 0), elem.FloatAttribute("index21", 0),
    };
}
UniformMatrix3x2f MaterialDeserializer::Helpers::DeserializeUniformMatrix3x2f(const tinyxml2::XMLElement& elem) noexcept {
    return {
        elem.FloatAttribute("index00", 0), elem.FloatAttribute("index01", 0), elem.FloatAttribute("index02", 0),
        elem.FloatAttribute("index10", 0), elem.FloatAttribute("index11", 0), elem.FloatAttribute("index12", 0),
    };
}
UniformMatrix2x4f MaterialDeserializer::Helpers::DeserializeUniformMatrix2x4f(const tinyxml2::XMLElement& elem) noexcept {
    return {
        elem.FloatAttribute("index00", 0), elem.FloatAttribute("index01", 0),
        elem.FloatAttribute("index10", 0), elem.FloatAttribute("index11", 0),
        elem.FloatAttribute("index20", 0), elem.FloatAttribute("index21", 0),
        elem.FloatAttribute("index30", 0), elem.FloatAttribute("index31", 0),
    };
}
UniformMatrix4x2f MaterialDeserializer::Helpers::DeserializeUniformMatrix4x2f(const tinyxml2::XMLElement& elem) noexcept {
    return {
        elem.FloatAttribute("index00", 0), elem.FloatAttribute("index01", 0), elem.FloatAttribute("index02", 0), elem.FloatAttribute("index03", 0),
        elem.FloatAttribute("index10", 0), elem.FloatAttribute("index11", 0), elem.FloatAttribute("index12", 0), elem.FloatAttribute("index13", 0),
    };
}
UniformMatrix3x4f MaterialDeserializer::Helpers::DeserializeUniformMatrix3x4f(const tinyxml2::XMLElement& elem) noexcept {
    return {
        elem.FloatAttribute("index00", 0), elem.FloatAttribute("index01", 0), elem.FloatAttribute("index02", 0),
        elem.FloatAttribute("index10", 0), elem.FloatAttribute("index11", 0), elem.FloatAttribute("index12", 0),
        elem.FloatAttribute("index20", 0), elem.FloatAttribute("index21", 0), elem.FloatAttribute("index22", 0),
        elem.FloatAttribute("index30", 0), elem.FloatAttribute("index31", 0), elem.FloatAttribute("index32", 0),
    };
}
UniformMatrix4x3f MaterialDeserializer::Helpers::DeserializeUniformMatrix4x3f(const tinyxml2::XMLElement& elem) noexcept {
    return {
        elem.FloatAttribute("index00", 0), elem.FloatAttribute("index01", 0), elem.FloatAttribute("index02", 0), elem.FloatAttribute("index03", 0),
        elem.FloatAttribute("index10", 0), elem.FloatAttribute("index11", 0), elem.FloatAttribute("index12", 0), elem.FloatAttribute("index13", 0),
        elem.FloatAttribute("index20", 0), elem.FloatAttribute("index21", 0), elem.FloatAttribute("index22", 0), elem.FloatAttribute("index23", 0),
    };
}