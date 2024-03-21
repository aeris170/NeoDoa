#include <Engine/ShaderDeserializer.hpp>

#include <regex>
#include <vector>
#include <cassert>

#include <Utility/Trim.hpp>

#include <Engine/FileNode.hpp>

static std::vector<std::string> SplitCompilerMessages(const std::string& messages) {
    static const std::regex regex{ "\n(?!\\s)" };

    std::vector<std::string> rv;
    std::copy(
        std::sregex_token_iterator{ messages.cbegin(), messages.cend(), regex, -1 },
        std::sregex_token_iterator{},
        std::back_inserter(rv)
    );

    return rv;
}

static ShaderCompilerMessage ParseCompilerMessage(const std::string& message) {
    static const std::regex regex{ "0\\((\\d*)\\)\\s*:\\s*(\\w*)\\s*\\w*\\s*:\\s*(.*)" };

    ShaderCompilerMessage rv;

    std::smatch match;
    if (std::regex_search(message, match, regex)) {
        rv.fullMessage = trim_copy(message);
        rv.lineNo = std::stoi(match[1]);
        std::string type = match[2];
        if (type == "info") {
            rv.messageType = ShaderCompilerMessageType::INFO;
        } else if (type == "warning") {
            rv.messageType = ShaderCompilerMessageType::WARNING;
        } else if (type == "error") {
            rv.messageType = ShaderCompilerMessageType::ERROR;
        } else {
            assert(false); // matches to something else, find and implement!
        }
        rv.shortMessage = match[3];
    } else {
        assert(false); // investigate why regex didn't match!
    }
    return rv;
}

static void QueryShaderCompilerMessages(GLuint shader, std::vector<ShaderCompilerMessage>& messages) {
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

ShaderDeserializationResult DeserializeVertexShader(const FNode& file) {
    file.ReadContent();
    auto rv = DeserializeVertexShader(file.DisposeContent());
    if (!rv.erred) {
        rv.deserializedShader.Name = file.Name();
    }
    return rv;
}
ShaderDeserializationResult DeserializeVertexShader(const std::string_view data) {
    ShaderDeserializationResult rv;
    rv.messages.emplace_back(0, ShaderCompilerMessageType::INFO, "Make sure this file contains GLSL Vertex Shader code!");

    const auto source = data.data();
    GLuint v = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v, 1, &source, NULL);

    int success;
    glCompileShader(v);
    glGetShaderiv(v, GL_COMPILE_STATUS, &success);
    if (!success) {
        rv.messages.emplace_back(0, ShaderCompilerMessageType::ERROR, "Vertex Shader compilation failed!");
    } else {
        rv.deserializedShader = { v, Shader::ShaderType::Vertex, "", source };
    }

    QueryShaderCompilerMessages(v, rv.messages);

    return rv;
}

ShaderDeserializationResult DeserializeTessellationControlShader(const FNode& file) {
    file.ReadContent();
    auto rv = DeserializeTessellationControlShader(file.DisposeContent());
    if (!rv.erred) {
        rv.deserializedShader.Name = file.Name();
    }
    return rv;
}
ShaderDeserializationResult DeserializeTessellationControlShader(const std::string_view data) {
    ShaderDeserializationResult rv;
    rv.messages.emplace_back(0, ShaderCompilerMessageType::INFO, "Make sure this file contains GLSL Tessellation Control Shader code!");

    const auto source = data.data();
    GLuint tc = glCreateShader(GL_TESS_CONTROL_SHADER);
    glShaderSource(tc, 1, &source, NULL);

    int success;
    glCompileShader(tc);
    glGetShaderiv(tc, GL_COMPILE_STATUS, &success);
    if (!success) {
        rv.messages.emplace_back(0, ShaderCompilerMessageType::ERROR, "Tessellation Control Shader compilation failed!");
    } else {
        rv.deserializedShader = { tc, Shader::ShaderType::TessellationControl, "", source };
    }


    QueryShaderCompilerMessages(tc, rv.messages);

    return rv;
}

ShaderDeserializationResult DeserializeTessellationEvaluationShader(const FNode& file) {
    file.ReadContent();
    auto rv = DeserializeTessellationEvaluationShader(file.DisposeContent());
    if (!rv.erred) {
        rv.deserializedShader.Name = file.Name();
    }
    return rv;
}
ShaderDeserializationResult DeserializeTessellationEvaluationShader(const std::string_view data) {
    ShaderDeserializationResult rv;
    rv.messages.emplace_back(0, ShaderCompilerMessageType::INFO, "Make sure this file contains GLSL Tessellation Evaluation Shader code!");

    const auto source = data.data();
    GLuint te = glCreateShader(GL_TESS_EVALUATION_SHADER);
    glShaderSource(te, 1, &source, NULL);

    int success;
    glCompileShader(te);
    glGetShaderiv(te, GL_COMPILE_STATUS, &success);
    if (!success) {
        rv.messages.emplace_back(0, ShaderCompilerMessageType::ERROR, "Tessellation Evaluation Shader compilation failed!");
    } else {
        rv.deserializedShader = { te, Shader::ShaderType::TessellationEvaluation, "", source };
    }


    QueryShaderCompilerMessages(te, rv.messages);

    return rv;
}

ShaderDeserializationResult DeserializeGeometryShader(const FNode& file) {
    file.ReadContent();
    auto rv = DeserializeGeometryShader(file.DisposeContent());
    if (!rv.erred) {
        rv.deserializedShader.Name = file.Name();
    }
    return rv;
}
ShaderDeserializationResult DeserializeGeometryShader(const std::string_view data) {
    ShaderDeserializationResult rv;
    rv.messages.emplace_back(0, ShaderCompilerMessageType::INFO, "Make sure this file contains GLSL Geometry Shader code!");

    const auto source = data.data();
    GLuint g = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(g, 1, &source, NULL);

    int success;
    glCompileShader(g);
    glGetShaderiv(g, GL_COMPILE_STATUS, &success);
    if (!success) {
        rv.messages.emplace_back(0, ShaderCompilerMessageType::ERROR, "Geometry Shader compilation failed!");
    } else {
        rv.deserializedShader = { g, Shader::ShaderType::Geometry, "", source };
    }


    QueryShaderCompilerMessages(g, rv.messages);

    return rv;
}

ShaderDeserializationResult DeserializeFragmentShader(const FNode& file) {
    file.ReadContent();
    auto rv = DeserializeFragmentShader(file.DisposeContent());
    if (!rv.erred) {
        rv.deserializedShader.Name = file.Name();
    }
    return rv;
}
ShaderDeserializationResult DeserializeFragmentShader(const std::string_view data) {
    ShaderDeserializationResult rv;
    rv.messages.emplace_back(0, ShaderCompilerMessageType::INFO, "Make sure this file contains GLSL Fragment Shader code!");

    const auto source = data.data();
    GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f, 1, &source, NULL);

    int success;
    glCompileShader(f);
    glGetShaderiv(f, GL_COMPILE_STATUS, &success);
    if (!success) {
        rv.messages.emplace_back(0, ShaderCompilerMessageType::ERROR, "Fragment Shader compilation failed!");
    } else {
        rv.deserializedShader = { f, Shader::ShaderType::Fragment, "", source };
    }


    QueryShaderCompilerMessages(f, rv.messages);

    return rv;
}

ShaderDeserializationResult DeserializeComputeShader(const FNode& file) {
    file.ReadContent();
    auto rv = DeserializeComputeShader(file.DisposeContent());
    if (!rv.erred) {
        rv.deserializedShader.Name = file.Name();
    }
    return rv;
}
ShaderDeserializationResult DeserializeComputeShader(const std::string_view data) {
    ShaderDeserializationResult rv;
    rv.messages.emplace_back(0, ShaderCompilerMessageType::INFO, "Make sure this file contains GLSL Compute Shader code!");

    const auto source = data.data();
    GLuint c = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(c, 1, &source, NULL);

    int success;
    glCompileShader(c);
    glGetShaderiv(c, GL_COMPILE_STATUS, &success);
    if (!success) {
        rv.messages.emplace_back(0, ShaderCompilerMessageType::ERROR, "Compute Shader compilation failed!");
    } else {
        rv.deserializedShader = { c, Shader::ShaderType::Compute, "", source };
    }


    QueryShaderCompilerMessages(c, rv.messages);

    return rv;
}
