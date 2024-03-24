#include <Engine/ShaderProgramDeserializer.hpp>

#include <regex>
#include <vector>
#include <format>
#include <string>

#include <Utility/Trim.hpp>

#include <Engine/Log.hpp>
#include <Engine/Core.hpp>
#include <Engine/FileNode.hpp>

static std::vector<std::string> SplitLinkerMessages(const std::string& messages) {
    static const std::regex regex{ "\n(?!\\s)" };

    std::vector<std::string> rv;
    std::copy(
        std::sregex_token_iterator{ messages.cbegin(), messages.cend(), regex, -1 },
        std::sregex_token_iterator{},
        std::back_inserter(rv)
    );

    return rv;
}
static std::string_view SymbolicConstantToShaderUniformType(GLint symbolicConstant) {
    if (symbolicConstant == GL_FLOAT)           { return "float"; }
    else if (symbolicConstant == GL_FLOAT_VEC2) { return "vec2";  }
    else if (symbolicConstant == GL_FLOAT_VEC3) { return "vec3";  }
    else if (symbolicConstant == GL_FLOAT_VEC4) { return "vec4";  }

    else if (symbolicConstant == GL_DOUBLE)      { return "double"; }
    else if (symbolicConstant == GL_DOUBLE_VEC2) { return "dvec2";  }
    else if (symbolicConstant == GL_DOUBLE_VEC3) { return "dvec3";  }
    else if (symbolicConstant == GL_DOUBLE_VEC4) { return "dvec4";  }

    else if (symbolicConstant == GL_INT)      { return "int";   }
    else if (symbolicConstant == GL_INT_VEC2) { return "ivec2"; }
    else if (symbolicConstant == GL_INT_VEC3) { return "ivec3"; }
    else if (symbolicConstant == GL_INT_VEC4) { return "ivec4"; }

    else if (symbolicConstant == GL_UNSIGNED_INT)      { return "unsigned int"; }
    else if (symbolicConstant == GL_UNSIGNED_INT_VEC2) { return "uvec2";        }
    else if (symbolicConstant == GL_UNSIGNED_INT_VEC3) { return "uvec3";        }
    else if (symbolicConstant == GL_UNSIGNED_INT_VEC4) { return "uvec4";        }

    else if (symbolicConstant == GL_BOOL)      { return "bool";  }
    else if (symbolicConstant == GL_BOOL_VEC2) { return "bvec2"; }
    else if (symbolicConstant == GL_BOOL_VEC3) { return "bvec3"; }
    else if (symbolicConstant == GL_BOOL_VEC4) { return "bvec4"; }

    else if (symbolicConstant == GL_FLOAT_MAT2)   { return "mat2";   }
    else if (symbolicConstant == GL_FLOAT_MAT3)   { return "mat3";   }
    else if (symbolicConstant == GL_FLOAT_MAT4)   { return "mat4";   }
    else if (symbolicConstant == GL_FLOAT_MAT2x3) { return "mat2x3"; }
    else if (symbolicConstant == GL_FLOAT_MAT2x4) { return "mat2x4"; }
    else if (symbolicConstant == GL_FLOAT_MAT3x2) { return "mat3x2"; }
    else if (symbolicConstant == GL_FLOAT_MAT3x4) { return "mat3x4"; }
    else if (symbolicConstant == GL_FLOAT_MAT4x2) { return "mat4x2"; }
    else if (symbolicConstant == GL_FLOAT_MAT4x3) { return "mat4x3"; }

    else if (symbolicConstant == GL_DOUBLE_MAT2)   { return "dmat2";   }
    else if (symbolicConstant == GL_DOUBLE_MAT3)   { return "dmat3";   }
    else if (symbolicConstant == GL_DOUBLE_MAT4)   { return "dmat4";   }
    else if (symbolicConstant == GL_DOUBLE_MAT2x3) { return "dmat2x3"; }
    else if (symbolicConstant == GL_DOUBLE_MAT2x4) { return "dmat2x4"; }
    else if (symbolicConstant == GL_DOUBLE_MAT3x2) { return "dmat3x2"; }
    else if (symbolicConstant == GL_DOUBLE_MAT3x4) { return "dmat3x4"; }
    else if (symbolicConstant == GL_DOUBLE_MAT4x2) { return "dmat4x2"; }
    else if (symbolicConstant == GL_DOUBLE_MAT4x3) { return "dmat4x3"; }

    else if (symbolicConstant == GL_SAMPLER_1D)                   { return "sampler1D";            }
    else if (symbolicConstant == GL_SAMPLER_2D)                   { return "sampler2D";            }
    else if (symbolicConstant == GL_SAMPLER_3D)                   { return "sampler3D";            }
    else if (symbolicConstant == GL_SAMPLER_CUBE)                 { return "samplerCube";          }
    else if (symbolicConstant == GL_SAMPLER_1D_SHADOW)            { return "sampler1DShadow";      }
    else if (symbolicConstant == GL_SAMPLER_2D_SHADOW)            { return "sampler2DShadow";      }
    else if (symbolicConstant == GL_SAMPLER_1D_ARRAY)             { return "sampler1DArray";       }
    else if (symbolicConstant == GL_SAMPLER_2D_ARRAY)             { return "sampler2DArray";       }
    else if (symbolicConstant == GL_SAMPLER_1D_ARRAY_SHADOW)      { return "sampler1DArrayShadow"; }
    else if (symbolicConstant == GL_SAMPLER_2D_ARRAY_SHADOW)      { return "sampler2DArrayShadow"; }
    else if (symbolicConstant == GL_SAMPLER_2D_MULTISAMPLE)       { return "sampler2DMS";          }
    else if (symbolicConstant == GL_SAMPLER_2D_MULTISAMPLE_ARRAY) { return "sampler2DMSArray";     }
    else if (symbolicConstant == GL_SAMPLER_CUBE_SHADOW)          { return "samplerCubeShadow";    }
    else if (symbolicConstant == GL_SAMPLER_BUFFER)               { return "samplerBuffer";        }
    else if (symbolicConstant == GL_SAMPLER_2D_RECT)              { return "sampler2DRect";        }
    else if (symbolicConstant == GL_SAMPLER_2D_RECT_SHADOW)       { return "sampler2DRectShadow";  }

    else if (symbolicConstant == GL_INT_SAMPLER_1D)                   { return "isampler1D";        }
    else if (symbolicConstant == GL_INT_SAMPLER_2D)                   { return "isampler2D";        }
    else if (symbolicConstant == GL_INT_SAMPLER_3D)                   { return "isampler3D";        }
    else if (symbolicConstant == GL_INT_SAMPLER_CUBE)                 { return "isamplerCube";      }
    else if (symbolicConstant == GL_INT_SAMPLER_1D_ARRAY)             { return "isampler1DArray";   }
    else if (symbolicConstant == GL_INT_SAMPLER_2D_ARRAY)             { return "isampler2DArray";   }
    else if (symbolicConstant == GL_INT_SAMPLER_2D_MULTISAMPLE)       { return "isampler2DMS";      }
    else if (symbolicConstant == GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY) { return "isampler2DMSArray"; }
    else if (symbolicConstant == GL_INT_SAMPLER_BUFFER)               { return "isamplerBuffer";    }
    else if (symbolicConstant == GL_INT_SAMPLER_2D_RECT)              { return "isampler2DRect";    }

    else if (symbolicConstant == GL_UNSIGNED_INT_SAMPLER_1D)                   { return "usampler1D";        }
    else if (symbolicConstant == GL_UNSIGNED_INT_SAMPLER_2D)                   { return "usampler2D";        }
    else if (symbolicConstant == GL_UNSIGNED_INT_SAMPLER_3D)                   { return "usampler3D";        }
    else if (symbolicConstant == GL_UNSIGNED_INT_SAMPLER_CUBE)                 { return "usamplerCube";      }
    else if (symbolicConstant == GL_UNSIGNED_INT_SAMPLER_1D_ARRAY)             { return "usampler2DArray";   }
    else if (symbolicConstant == GL_UNSIGNED_INT_SAMPLER_2D_ARRAY)             { return "usampler2DArray";   }
    else if (symbolicConstant == GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE)       { return "usampler2DMS";      }
    else if (symbolicConstant == GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY) { return "usampler2DMSArray"; }
    else if (symbolicConstant == GL_UNSIGNED_INT_SAMPLER_BUFFER)               { return "usamplerBuffer";    }
    else if (symbolicConstant == GL_UNSIGNED_INT_SAMPLER_2D_RECT)              { return "usampler2DRect";    }

    else if (symbolicConstant == GL_IMAGE_1D)                   { return "image1D";        }
    else if (symbolicConstant == GL_IMAGE_2D)                   { return "image2D";        }
    else if (symbolicConstant == GL_IMAGE_3D)                   { return "image3D";        }
    else if (symbolicConstant == GL_IMAGE_2D_RECT)              { return "image2DRect";    }
    else if (symbolicConstant == GL_IMAGE_CUBE)                 { return "imageCube";      }
    else if (symbolicConstant == GL_IMAGE_BUFFER)               { return "imageBuffer";    }
    else if (symbolicConstant == GL_IMAGE_1D_ARRAY)             { return "image1DArray";   }
    else if (symbolicConstant == GL_IMAGE_2D_ARRAY)             { return "image2DArray";   }
    else if (symbolicConstant == GL_IMAGE_2D_MULTISAMPLE)       { return "image2DMS";      }
    else if (symbolicConstant == GL_IMAGE_2D_MULTISAMPLE_ARRAY) { return "image2DMSArray"; }

    else if (symbolicConstant == GL_INT_IMAGE_1D)                   { return "iimage1D";        }
    else if (symbolicConstant == GL_INT_IMAGE_2D)                   { return "iimage2D";        }
    else if (symbolicConstant == GL_INT_IMAGE_3D)                   { return "iimage3D";        }
    else if (symbolicConstant == GL_INT_IMAGE_2D_RECT)              { return "iimage2DRect";    }
    else if (symbolicConstant == GL_INT_IMAGE_CUBE)                 { return "iimageCube";      }
    else if (symbolicConstant == GL_INT_IMAGE_BUFFER)               { return "iimageBuffer";    }
    else if (symbolicConstant == GL_INT_IMAGE_1D_ARRAY)             { return "iimage1DArray";   }
    else if (symbolicConstant == GL_INT_IMAGE_2D_ARRAY)             { return "iimage2DArray";   }
    else if (symbolicConstant == GL_INT_IMAGE_2D_MULTISAMPLE)       { return "iimage2DMS";      }
    else if (symbolicConstant == GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY) { return "iimage2DMSArray"; }

    else if (symbolicConstant == GL_UNSIGNED_INT_IMAGE_1D)                   { return "uimage1D";        }
    else if (symbolicConstant == GL_UNSIGNED_INT_IMAGE_2D)                   { return "uimage2D";        }
    else if (symbolicConstant == GL_UNSIGNED_INT_IMAGE_3D)                   { return "uimage3D";        }
    else if (symbolicConstant == GL_UNSIGNED_INT_IMAGE_2D_RECT)              { return "uimage2DRect";    }
    else if (symbolicConstant == GL_UNSIGNED_INT_IMAGE_CUBE)                 { return "uimageCube";      }
    else if (symbolicConstant == GL_UNSIGNED_INT_IMAGE_BUFFER)               { return "uimageBuffer";    }
    else if (symbolicConstant == GL_UNSIGNED_INT_IMAGE_1D_ARRAY)             { return "uimage1DArray";   }
    else if (symbolicConstant == GL_UNSIGNED_INT_IMAGE_2D_ARRAY)             { return "uimage2DArray";   }
    else if (symbolicConstant == GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE)       { return "uimage2DMS";      }
    else if (symbolicConstant == GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY) { return "uimage2DMSArray"; }

    else if (symbolicConstant == GL_UNSIGNED_INT_ATOMIC_COUNTER) { return "atomic_uint"; }

    else {
        DOA_LOG_WARNING("Detected unknown type of uniform. Symbolic constant: %d", symbolicConstant);
        return "NEODOA_SHADER_UNIFORM_UNKNOWN_TYPE";
    }
}

ShaderProgramDeserializationResult DeserializeShaderProgram(const FNode& file) {
    file.ReadContent();
    return DeserializeShaderProgram(file.DisposeContent());
}
ShaderProgramDeserializationResult DeserializeShaderProgram(std::string_view data) {
    ShaderProgramDeserializationResult rv;

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError err = doc.Parse(data.data());
    if (err != tinyxml2::XML_SUCCESS) {
        rv.erred = true;
        rv.errors.emplace_back("Error while parsing program file. Did you edit it? Don't.");
        DOA_LOG_ERROR("Couldn't deserialize shader program!\n\n%s", data);
        return rv;
    }

    ShaderProgramDeserializer::Deserialize(*doc.RootElement(), rv);
    return rv;
}

void ShaderProgramDeserializer::DefaultDeserialize(tinyxml2::XMLElement& rootNode, ShaderProgramDeserializationResult& spdr) {
    ProgramConfig::Deserialize(*rootNode.FirstChildElement("config"), spdr);
    Shaders::Deserialize(*rootNode.FirstChildElement("shaders"), spdr);
    Linking::Link(spdr);
}

void ShaderProgramDeserializer::ProgramConfig::DefaultDeserialize(tinyxml2::XMLElement& configNode, ShaderProgramDeserializationResult& spdr) {
    DeserializeName(configNode, spdr);
}
void ShaderProgramDeserializer::ProgramConfig::DefaultDeserializeName(tinyxml2::XMLElement& configNode, ShaderProgramDeserializationResult& spdr) {
    spdr.deserializedShaderProgram.Name = configNode.Attribute("name");
}

void ShaderProgramDeserializer::Shaders::DefaultDeserialize(tinyxml2::XMLElement& shadersNode, ShaderProgramDeserializationResult& spdr) {
    tinyxml2::XMLElement* node;

    node = shadersNode.FirstChildElement("vertex-shader");
    if (node == nullptr) {
        spdr.erred = true;
        spdr.errors.emplace_back(std::format("Can't find node \"{}-shader\" in program file.", "vertex"));
    }
    DeserializeVertexShader(*node, spdr);

    node = shadersNode.FirstChildElement("tess-ctrl-shader");
    if (node == nullptr) {
        spdr.erred = true;
        spdr.errors.emplace_back(std::format("Can't find node \"{}-shader\" in program file.", "tess-ctrl"));
    }
    DeserializeTessellationControlShader(*node, spdr);

    node = shadersNode.FirstChildElement("tess-eval-shader");
    if (node == nullptr) {
        spdr.erred = true;
        spdr.errors.emplace_back(std::format("Can't find node \"{}-shader\" in program file.", "tess-eval"));
    }
    DeserializeTessellationEvaluationShader(*node, spdr);

    node = shadersNode.FirstChildElement("geometry-shader");
    if (node == nullptr) {
        spdr.erred = true;
        spdr.errors.emplace_back(std::format("Can't find node \"{}-shader\" in program file.", "geometry"));
    }
    DeserializeGeometryShader(*node, spdr);

    node = shadersNode.FirstChildElement("fragment-shader");
    if (node == nullptr) {
        spdr.erred = true;
        spdr.errors.emplace_back(std::format("Can't find node \"{}-shader\" in program file.", "fragment"));
    }
    DeserializeFragmentShader(*node, spdr);
    //DeserializeVertexShader(shadersNode.FirstChildElement("compute-shader"), program);
}
void ShaderProgramDeserializer::Shaders::DefaultDeserializeVertexShader(tinyxml2::XMLElement& vertexShaderNode, ShaderProgramDeserializationResult& spdr) {
    spdr.deserializedShaderProgram.VertexShader = static_cast<UUID>(vertexShaderNode.Unsigned64Attribute("uuid", UUID::Empty()));
}
void ShaderProgramDeserializer::Shaders::DefaultDeserializeTessellationControlShader(tinyxml2::XMLElement& tessCtrlShaderNode, ShaderProgramDeserializationResult& spdr) {
    spdr.deserializedShaderProgram.TessellationControlShader = static_cast<UUID>(tessCtrlShaderNode.Unsigned64Attribute("uuid", UUID::Empty()));
}
void ShaderProgramDeserializer::Shaders::DefaultDeserializeTessellationEvaluationShader(tinyxml2::XMLElement& tessEvalShaderNode, ShaderProgramDeserializationResult& spdr) {
    spdr.deserializedShaderProgram.TessellationEvaluationShader = static_cast<UUID>(tessEvalShaderNode.Unsigned64Attribute("uuid", UUID::Empty()));
}
void ShaderProgramDeserializer::Shaders::DefaultDeserializeGeometryShader(tinyxml2::XMLElement& geometryShaderNode, ShaderProgramDeserializationResult& spdr) {
    spdr.deserializedShaderProgram.GeometryShader = static_cast<UUID>(geometryShaderNode.Unsigned64Attribute("uuid", UUID::Empty()));
}
void ShaderProgramDeserializer::Shaders::DefaultDeserializeFragmentShader(tinyxml2::XMLElement& fragmentShaderNode, ShaderProgramDeserializationResult& spdr) {
    spdr.deserializedShaderProgram.FragmentShader = static_cast<UUID>(fragmentShaderNode.Unsigned64Attribute("uuid", UUID::Empty()));
}
void ShaderProgramDeserializer::Shaders::DefaultDeserializeComputeShader(tinyxml2::XMLElement& computeShaderNode, ShaderProgramDeserializationResult& spdr) {
    //program.vertexShader = computeShaderNode.FindAttribute("id")->Unsigned64Attribute();
}

void ShaderProgramDeserializer::Linking::DefaultLink(ShaderProgramDeserializationResult& spdr) {
    if (spdr.erred) { return; } /* if we have errors, don't even try to link program. */
    OpenGLLink(spdr);
}
void ShaderProgramDeserializer::Linking::DefaultOpenGLLink(ShaderProgramDeserializationResult& spdr) {
    ShaderProgram& program = spdr.deserializedShaderProgram;
    if (!program.IsComplete()) {
        if (!program.HasVertexShader()) {
            spdr.erred = true;
            spdr.errors.emplace_back("Can't link program, program does not have a vertex shader!");
        }
        if (!program.HasFragmentShader()) {
            spdr.erred = true;
            spdr.errors.emplace_back("Can't link program, program does not have a fragment shader!");
        }
    }
    GLuint ID = glCreateProgram();
    std::array<GLuint, 5> attachedShaders{};

    const Assets& assets = *Core::GetCore()->GetAssets().get();

    // attach mandatory shaders - vertex and fragment
    AssetHandle vertexShaderAsset = assets.FindAsset(program.VertexShader);
    if (!vertexShaderAsset.HasValue()) {
        spdr.erred = true;
        spdr.errors.emplace_back("Can't link program, program references a non-existant vertex shader, has your shader successfully deserialized?");
    }
    AssetHandle fragmentShaderAsset = assets.FindAsset(program.FragmentShader);
    if (!fragmentShaderAsset.HasValue()) {
        spdr.erred = true;
        spdr.errors.emplace_back("Can't link program, program references a non-existant fragment shader, has your shader successfully deserialized?");
    }
    if (!vertexShaderAsset.HasValue() || !fragmentShaderAsset.HasValue()) {
        spdr.errors.emplace_back("Please fix errors before continuing.");
        return;
    }
    const Shader& vertexShader = vertexShaderAsset->DataAs<Shader>();
    glAttachShader(ID, vertexShader.ID);
    const Shader& fragmentShader = fragmentShaderAsset->DataAs<Shader>();
    glAttachShader(ID, fragmentShader.ID);
    attachedShaders[0] = vertexShader.ID;
    attachedShaders[4] = fragmentShader.ID;

    // attach optional shaders - tess_ctrl, tess_eval and geometry
    bool usingTessellation{ program.HasTessellationEvaluationShader() };
    if (usingTessellation && program.HasTessellationControlShader()) {
        AssetHandle tessCtrlShaderAsset = assets.FindAsset(program.TessellationControlShader);
        if (tessCtrlShaderAsset.HasValue()) {
            const Shader& tessCtrlShader = tessCtrlShaderAsset->DataAs<Shader>();
            glAttachShader(ID, tessCtrlShader.ID);
            attachedShaders[1] = tessCtrlShader.ID;
        }
    }
    if (usingTessellation) {
        AssetHandle tessEvalShaderAsset = assets.FindAsset(program.TessellationEvaluationShader);
        if (!tessEvalShaderAsset.HasValue()) {
            spdr.erred = true;
            spdr.errors.emplace_back("Can't link program, program uses tessellation but references a non-existant evaluation shader, has your shader successfully deserialized?");
            return;
        }
        const Shader& tessEvalShader = tessEvalShaderAsset->DataAs<Shader>();
        glAttachShader(ID, tessEvalShader.ID);
        attachedShaders[2] = tessEvalShader.ID;
    }
    if (program.HasGeometryShader()) {
        AssetHandle geometryShaderAsset = assets.FindAsset(program.GeometryShader);
        if (!geometryShaderAsset.HasValue()) {
            spdr.erred = true;
            spdr.errors.emplace_back("Can't link program, program references a non-existant geometry shader, has your shader successfully deserialized?");
            return;
        }
        const Shader& geometryShader = geometryShaderAsset->DataAs<Shader>();
        glAttachShader(ID, geometryShader.ID);
        attachedShaders[3] = geometryShader.ID;
    }

    int success;
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        spdr.erred = true;
        spdr.errors.emplace_back("Program linking failed!");
        glDeleteProgram(ID);
    } else {
        program.ID = ID;
    }

    GLint bufferLength;
    glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &bufferLength);
    if (bufferLength > 1) {
        GLchar* logChars = new char[bufferLength + 1];
        glGetProgramInfoLog(ID, bufferLength, NULL, logChars);
        std::string logString{ logChars };
        trim(logString);
        auto logs = SplitLinkerMessages(logString);
        for (auto& log : logs) {
            spdr.errors.emplace_back(std::move(log));
        }
        delete[] logChars;
    }

    // Extract active uniforms
    program.Uniforms.clear();
    GLint numActiveUniforms = 0;
    glGetProgramInterfaceiv(ID, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveUniforms);
    std::vector<GLchar> nameData(256);
    std::vector<GLenum> properties;
    properties.push_back(GL_NAME_LENGTH);                          // 0
    properties.push_back(GL_TYPE);                                 // 1
    properties.push_back(GL_ARRAY_SIZE);                           // 2
    properties.push_back(GL_REFERENCED_BY_VERTEX_SHADER);          // 3
    properties.push_back(GL_REFERENCED_BY_TESS_CONTROL_SHADER);    // 4
    properties.push_back(GL_REFERENCED_BY_TESS_EVALUATION_SHADER); // 5
    properties.push_back(GL_REFERENCED_BY_GEOMETRY_SHADER);        // 6
    properties.push_back(GL_REFERENCED_BY_FRAGMENT_SHADER);        // 7
    std::vector<GLint> values(properties.size());

    for (int i = 0; i < numActiveUniforms; i++) {
        ShaderProgram::Uniform uniform;
        glGetProgramResourceiv(ID, GL_UNIFORM, i, properties.size(), properties.data(), values.size(), NULL, values.data());

        // Extract uniform name
        nameData.resize(values[0]); //The length of the name.
        glGetProgramResourceName(ID, GL_UNIFORM, i, nameData.size(), NULL, nameData.data());
        uniform.Name = std::string(nameData.data(), nameData.size() - 1);

        // Extract uniform location
        uniform.Location = glGetUniformLocation(ID, uniform.Name.c_str());

        // Extract uniform type name
        uniform.TypeName = SymbolicConstantToShaderUniformType(values[1]);

        // Extract uniform array size (eg. extracts 100 from "mat4 bones[100]")
        uniform.ArraySize = values[2];

        // Extract uniform source (which shader it comes from)
        if (values[3]) {
            uniform.ReferencedBy = Shader::ShaderType::Vertex;
        } else if (values[4]) {
            uniform.ReferencedBy = Shader::ShaderType::TessellationControl;
        } else if (values[5]) {
            uniform.ReferencedBy = Shader::ShaderType::TessellationEvaluation;
        } else if (values[6]) {
            uniform.ReferencedBy = Shader::ShaderType::Geometry;
        } else if (values[7]) {
            uniform.ReferencedBy = Shader::ShaderType::Fragment;
        } else {
            spdr.erred = true;
            spdr.errors.emplace_back(std::format("Uniform {} not referenced by any shader.", uniform.Name));
            continue; // error! abort this uniform and continue with the next one.
        }

        int idx = 0;
        while(idx < uniform.ArraySize) {
            spdr.deserializedShaderProgram.Uniforms.emplace_back(uniform);

            uniform.Location++;
            uniform.Name = std::format("{}[{}]", uniform.Name.substr(0, uniform.Name.size() - 3), idx + 1);
            idx++;
        }
    }

    for (const auto shader : attachedShaders) {
        if (shader == 0) { continue; }
        glDetachShader(ID, shader);
    }
}