#include <Engine/GPUShader.hpp>

#include <regex>
#include <format>
#include <iomanip>
#include <cassert>

#include <Utility/Trim.hpp>

#include <Engine/Log.hpp>

// Shader
GPUShader::~GPUShader() noexcept {
    glDeleteShader(GLObjectID);
}
GPUShader::GPUShader(GPUShader&& other) noexcept {
    *this = std::move(other);
}
GPUShader& GPUShader::operator=(GPUShader&& other) noexcept {
    std::swap(GLObjectID, other.GLObjectID);
    Type = std::exchange(other.Type, {});
#ifdef DEBUG
    Name = std::move(other.Name);
#endif
    return *this;
}

GPUShaderBuilder& GPUShaderBuilder::SetType(ShaderType shaderType) noexcept {
    type = shaderType;
    return *this;
}
GPUShaderBuilder& GPUShaderBuilder::SetName(std::string_view shaderName) noexcept {
#ifdef DEBUG
    name = shaderName;
#endif
    return *this;
}
GPUShaderBuilder& GPUShaderBuilder::SetSourceCode(std::string_view code) noexcept {
    sourceCode = code;
    return *this;
}
std::pair<std::optional<GPUShader>, std::vector<ShaderCompilerMessage>> GPUShaderBuilder::Build() noexcept {
    std::vector<ShaderCompilerMessage> messages{};
    messages.emplace_back(0, ShaderCompilerMessageType::Info, std::format("Make sure this file contains GLSL {} Shader code!", ToString(type)));

    GLuint shader{};
    switch (type) {
    using enum ShaderType;
    case Vertex:                 shader = glCreateShader(GL_VERTEX_SHADER);          break;
    case TessellationControl:    shader = glCreateShader(GL_TESS_CONTROL_SHADER);    break;
    case TessellationEvaluation: shader = glCreateShader(GL_TESS_EVALUATION_SHADER); break;
    case Geometry:               shader = glCreateShader(GL_GEOMETRY_SHADER);        break;
    case Fragment:               shader = glCreateShader(GL_FRAGMENT_SHADER);        break;
    case Compute:                shader = glCreateShader(GL_COMPUTE_SHADER);         break;
    }
    if (shader == decltype(shader){}) {
        // switch-case failed / user entered out of bounds type!
        messages.emplace_back(0, ShaderCompilerMessageType::Error, std::format("{} Shader compilation failed!", ToString(type)));
        messages.emplace_back(0, ShaderCompilerMessageType::Error, "Couldn't make out shader type!");
        return { std::nullopt, messages };
    }

    const auto source = sourceCode.data();
    glShaderSource(shader, 1, &source, nullptr);

    int success;
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        messages.emplace_back(0, ShaderCompilerMessageType::Error, std::format("{} Shader compilation failed!", ToString(type)));
        glDeleteShader(shader);
    }

    QueryShaderCompilerMessages(shader, messages);

    std::optional<GPUShader> gpuShader{ std::nullopt };
    if (success) {
        gpuShader.emplace();
        gpuShader->GLObjectID = shader;
        gpuShader->Type = type;
#ifdef DEBUG
        gpuShader->Name = std::move(name);
#endif
    }

    return { std::move(gpuShader), std::move(messages) };
}

std::vector<std::string> GPUShaderBuilder::SplitCompilerMessages(const std::string& messages) noexcept {
    static const std::regex regex{ "\n(?!\\s)" };

    std::vector<std::string> rv;
    std::copy(
        std::sregex_token_iterator{ messages.cbegin(), messages.cend(), regex, -1 },
        std::sregex_token_iterator{},
        std::back_inserter(rv)
    );

    return rv;
}
ShaderCompilerMessage GPUShaderBuilder::ParseCompilerMessage(const std::string& message) noexcept {
    static const std::regex regex{ "0\\((\\d*)\\)\\s*:\\s*(\\w*)\\s*\\w*\\s*:\\s*(.*)" };

    ShaderCompilerMessage rv;

    std::smatch match;
    if (std::regex_search(message, match, regex)) {
        rv.FullMessage = trim_copy(message);
        rv.LineNo = std::stoi(match[1]);
        std::string type = match[2];
        if (type == "info") {
            rv.MessageType = ShaderCompilerMessageType::Info;
        } else if (type == "warning") {
            rv.MessageType = ShaderCompilerMessageType::Warning;
        } else if (type == "error") {
            rv.MessageType = ShaderCompilerMessageType::Error;
        } else {
            assert(false); // matches to something else, find and implement!
        }
        rv.ShortMessage = match[3];
    } else {
        assert(false); // investigate why regex didn't match!
    }
    return rv;
}
void GPUShaderBuilder::QueryShaderCompilerMessages(GLuint shader, std::vector<ShaderCompilerMessage>& messages) noexcept {
    GLint bufferLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufferLength);
    if (bufferLength > 1) {
        GLchar* logChars = new char[bufferLength + 1];
        glGetShaderInfoLog(shader, bufferLength, NULL, logChars);
        std::string logString{ logChars };
        trim(logString);
        auto logs = SplitCompilerMessages(logString);
        for (const auto& log : logs) {
            messages.emplace_back(ParseCompilerMessage(log));
        }
        delete[] logChars;
    }
}

// Shader Program
GPUShaderProgram::~GPUShaderProgram() noexcept {
    glDeleteProgram(GLObjectID);
}
GPUShaderProgram::GPUShaderProgram(GPUShaderProgram&& other) noexcept {
    *this = std::move(other);
}
GPUShaderProgram& GPUShaderProgram::operator=(GPUShaderProgram&& other) noexcept {
    std::swap(GLObjectID, other.GLObjectID);
#ifdef DEBUG
    Name = std::move(other.Name);
#endif
    Uniforms = std::move(other.Uniforms);
    return *this;
}

int GPUShaderProgram::GetUniformLocation(std::string_view name) const noexcept {
    auto search = std::ranges::find_if(Uniforms, [name](const Uniform& uniform) { return uniform.Name == name; });
#ifdef DEBUG
    if (search == Uniforms.end()) {
        DOA_LOG_WARNING("Program %s does not contain uniform %s", std::quoted(Name), std::quoted(name));
        return -1;
    }
#endif
    return search->Location;
}

GPUShaderProgramBuilder& GPUShaderProgramBuilder::SetName(const std::string_view programName) noexcept {
#ifdef DEBUG
    name = programName;
#endif
    return *this;
}
GPUShaderProgramBuilder& GPUShaderProgramBuilder::SetVertexShader(GPUShader& shader) noexcept {
    vertShader = &shader;
    return *this;
}
GPUShaderProgramBuilder& GPUShaderProgramBuilder::SetTessellationControlShader(GPUShader& shader) noexcept {
    tessCtrlShader = &shader;
    return *this;
}
GPUShaderProgramBuilder& GPUShaderProgramBuilder::SetTessellationEvaluationShader(GPUShader& shader) noexcept {
    tessEvalShader = &shader;
    return *this;
}
GPUShaderProgramBuilder& GPUShaderProgramBuilder::SetGeometryShader(GPUShader& shader) noexcept {
    geomShader = &shader;
    return *this;
}
GPUShaderProgramBuilder& GPUShaderProgramBuilder::SetFragmentShader(GPUShader& shader) noexcept {
    fragShader = &shader;
    return *this;
}
GPUShaderProgramBuilder& GPUShaderProgramBuilder::SetComputeShader(GPUShader& shader) noexcept {
    compShader = &shader;
    return *this;
}
std::pair<std::optional<GPUShaderProgram>, std::vector<ShaderLinkerMessage>> GPUShaderProgramBuilder::Build() noexcept {
    if (compShader) {
        return BuildComputePipeline();
    } else if (vertShader && fragShader) {
        return BuildGraphicsPipeline();
    } else {
        return { std::nullopt, { "No pipeline can be built with shaders set.", "Make sure you set either both Vertex and Fragment shader or the Compute Shader alone." } };
    }
}

std::pair<std::optional<GPUShaderProgram>, std::vector<ShaderLinkerMessage>> GPUShaderProgramBuilder::BuildGraphicsPipeline() noexcept {
    std::vector<ShaderLinkerMessage> messages{};
    if (!vertShader) {
        messages.emplace_back("Vertex shader cannot be null while building a graphics pipeline.");
        return { std::nullopt, std::move(messages) };
    }
    if (!fragShader) {
        messages.emplace_back("Fragment shader cannot be null while building a graphics pipeline.");
        return { std::nullopt, std::move(messages) };
    }

    GLuint program = glCreateProgram();

    // Attach pipeline stages
    glAttachShader(program, vertShader->GLObjectID);
    if (tessCtrlShader) {
        glAttachShader(program, tessCtrlShader->GLObjectID);
    }
    if (tessEvalShader) {
        glAttachShader(program, tessEvalShader->GLObjectID);
    }
    if (geomShader) {
        glAttachShader(program, geomShader->GLObjectID);
    }
    glAttachShader(program, fragShader->GLObjectID);

    // Link program
    bool success = LinkProgram(program, messages);

    std::optional<GPUShaderProgram> gpuShaderProgram{ std::nullopt };
    if (success) {
        gpuShaderProgram.emplace();
        gpuShaderProgram->GLObjectID = program;
#ifdef DEBUG
        gpuShaderProgram->Name = std::move(name);
#endif
        gpuShaderProgram->Uniforms = ExtractActiveProgramUniforms(program, messages);
    }

    // Detach pipeline stages
    glDetachShader(program, vertShader->GLObjectID);
    if (tessCtrlShader) {
        glDetachShader(program, tessCtrlShader->GLObjectID);
    }
    if (tessEvalShader) {
        glDetachShader(program, tessEvalShader->GLObjectID);
    }
    if (geomShader) {
        glDetachShader(program, geomShader->GLObjectID);
    }
    glDetachShader(program, fragShader->GLObjectID);

    return { std::move(gpuShaderProgram), std::move(messages) };
}
std::pair<std::optional<GPUShaderProgram>, std::vector<ShaderLinkerMessage>> GPUShaderProgramBuilder::BuildComputePipeline() noexcept {
    std::vector<ShaderLinkerMessage> messages{};
    if (!compShader) {
        messages.emplace_back("Compute shader cannot be null while building a compute pipeline.");
        return { std::nullopt, std::move(messages) };
    }

    GLuint program = glCreateProgram();

    // Attach compute stage
    glAttachShader(program, compShader->GLObjectID);

    // Link program
    bool success = LinkProgram(program, messages);

    std::optional<GPUShaderProgram> gpuShaderProgram{ std::nullopt };
    if (success) {
        gpuShaderProgram.emplace();
        gpuShaderProgram->GLObjectID = program;
#ifdef DEBUG
        gpuShaderProgram->Name = std::move(name);
#endif
        gpuShaderProgram->Uniforms = ExtractActiveProgramUniforms(program, messages);
    }

    // Detach compute stage
    glDetachShader(program, compShader->GLObjectID);

    return { std::move(gpuShaderProgram), std::move(messages) };
}

std::vector<std::string> GPUShaderProgramBuilder::SplitLinkerMessages(const std::string& messages) noexcept {
    static const std::regex regex{ "\n(?!\\s)" };

    std::vector<std::string> rv;
    std::copy(
        std::sregex_token_iterator{ messages.cbegin(), messages.cend(), regex, -1 },
        std::sregex_token_iterator{},
        std::back_inserter(rv)
    );

    return rv;
}
std::string_view GPUShaderProgramBuilder::SymbolicConstantToShaderUniformType(GLint symbolicConstant) noexcept {
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
        std::unreachable();
    }
}
bool GPUShaderProgramBuilder::LinkProgram(GLuint program, std::vector<ShaderLinkerMessage>& messages) noexcept {
    GLint success;

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        messages.emplace_back("Program linking failed!");

        GLint bufferLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufferLength);
        if (bufferLength > 1) {
            GLchar* logChars = new char[bufferLength + 1];
            glGetProgramInfoLog(program, bufferLength, NULL, logChars);
            std::string logString{ logChars };
            trim(logString);
            auto logs = SplitLinkerMessages(logString);
            for (auto& log : logs) {
                messages.emplace_back(std::move(log));
            }
            delete[] logChars;
        }

        glDeleteProgram(program);
    }

    return static_cast<bool>(success);
}
std::vector<GPUShaderProgram::Uniform> GPUShaderProgramBuilder::ExtractActiveProgramUniforms(GLuint program, std::vector<ShaderLinkerMessage>& messages) noexcept {
    std::vector<GPUShaderProgram::Uniform> rv;

    // Extract active uniforms
    GLint numActiveUniforms = 0;
    glGetProgramInterfaceiv(program, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveUniforms);
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
    properties.push_back(GL_REFERENCED_BY_COMPUTE_SHADER);         // 8
    std::vector<GLint> values(properties.size());

    for (int i = 0; i < numActiveUniforms; i++) {
        GPUShaderProgram::Uniform uniform;
        glGetProgramResourceiv(program, GL_UNIFORM, i, properties.size(), properties.data(), values.size(), NULL, values.data());

        // Extract uniform name
        nameData.resize(values[0]); //The length of the name.
        glGetProgramResourceName(program, GL_UNIFORM, i, nameData.size(), NULL, nameData.data());
        uniform.Name = std::string(nameData.data(), nameData.size() - 1);

        // Extract uniform location
        uniform.Location = glGetUniformLocation(program, uniform.Name.c_str());

        // Extract uniform type name
        uniform.TypeName = SymbolicConstantToShaderUniformType(values[1]);

        // Extract uniform array size (eg. extracts 100 from "mat4 bones[100]")
        uniform.ArraySize = values[2];

        // Extract uniform source (which shader it comes from)
        if (values[3]) {
            uniform.ReferencedBy = ShaderType::Vertex;
        } else if (values[4]) {
            uniform.ReferencedBy = ShaderType::TessellationControl;
        } else if (values[5]) {
            uniform.ReferencedBy = ShaderType::TessellationEvaluation;
        } else if (values[6]) {
            uniform.ReferencedBy = ShaderType::Geometry;
        } else if (values[7]) {
            uniform.ReferencedBy = ShaderType::Fragment;
        } else if (values[8]) {
            uniform.ReferencedBy = ShaderType::Compute;
        } else {
            messages.emplace_back(std::format("Uniform {} not referenced by any shader.", uniform.Name));
            continue; // error! abort this uniform and continue with the next one.
        }

        int idx = 0;
        while (idx < uniform.ArraySize) {
            rv.emplace_back(uniform);

            uniform.Location++;
            uniform.Name = std::format("{}[{}]", uniform.Name.substr(0, uniform.Name.size() - 3), idx + 1);
            idx++;
        }
    }

    return rv;
}