#include <Engine/ShaderDeserializer.hpp>

#include <regex>
#include <vector>
#include <sstream>

#include <Utility/trim.hpp>
#include <Utility/split.hpp>

#include "Core.hpp"
#include "Angel.hpp"
#include "Log.hpp"

#include "FileNode.hpp"

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

ShaderDeserializationResult DeserializeVertexShader(const FNode& file) {
    file.ReadContent();
    auto rv = DeserializeVertexShader(file.DisposeContent());
    if (!rv.erred) {
        rv.deserializedShader.name = file.Name();
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
        rv.deserializedShader = { v, Shader::Type::VERTEX, "", source, {} };
    }

    GLint bufferLength;
    glGetShaderiv(v, GL_INFO_LOG_LENGTH, &bufferLength);
    if (bufferLength > 1) {
        GLchar* logChars = new char[bufferLength + 1];
        glGetShaderInfoLog(v, bufferLength, NULL, logChars);
        std::string logString{ logChars };
        trim(logString);
        auto logs = SplitCompilerMessages(logString);
        for (const auto& log : logs) {
            rv.messages.emplace_back(ParseCompilerMessage(log));
        }
        delete[] logChars;
    }

    return rv;
}

ShaderDeserializationResult DeserializeTessellationControlShader(const FNode& file) {
    file.ReadContent();
    return DeserializeTessellationControlShader(file.DisposeContent());
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
        rv.deserializedShader = { tc, Shader::Type::TESS_CTRL, "", source, {} };
    }

    GLint bufferLength;
    glGetShaderiv(tc, GL_INFO_LOG_LENGTH, &bufferLength);
    if (bufferLength > 1) {
        GLchar* logChars = new char[bufferLength + 1];
        glGetShaderInfoLog(tc, bufferLength, NULL, logChars);
        std::string logString{ logChars };
        trim(logString);
        auto logs = SplitCompilerMessages(logString);
        for (const auto& log : logs) {
            rv.messages.emplace_back(ParseCompilerMessage(log));
        }
        delete[] logChars;
    }

    return rv;
}

ShaderDeserializationResult DeserializeTessellationEvaluationShader(const FNode& file) {
    file.ReadContent();
    return DeserializeTessellationEvaluationShader(file.DisposeContent());
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
        rv.deserializedShader = { te, Shader::Type::TESS_EVAL, "", source, {} };
    }

    GLint bufferLength;
    glGetShaderiv(te, GL_INFO_LOG_LENGTH, &bufferLength);
    if (bufferLength > 1) {
        GLchar* logChars = new char[bufferLength + 1];
        glGetShaderInfoLog(te, bufferLength, NULL, logChars);
        std::string logString{ logChars };
        trim(logString);
        auto logs = SplitCompilerMessages(logString);
        for (const auto& log : logs) {
            rv.messages.emplace_back(ParseCompilerMessage(log));
        }
        delete[] logChars;
    }

    return rv;
}

ShaderDeserializationResult DeserializeGeometryShader(const FNode& file) {
    file.ReadContent();
    return DeserializeGeometryShader(file.DisposeContent());
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
        rv.deserializedShader = { g, Shader::Type::GEOMETRY, "", source, {} };
    }

    GLint bufferLength;
    glGetShaderiv(g, GL_INFO_LOG_LENGTH, &bufferLength);
    if (bufferLength > 1) {
        GLchar* logChars = new char[bufferLength + 1];
        glGetShaderInfoLog(g, bufferLength, NULL, logChars);
        std::string logString{ logChars };
        trim(logString);
        auto logs = SplitCompilerMessages(logString);
        for (const auto& log : logs) {
            rv.messages.emplace_back(ParseCompilerMessage(log));
        }
        delete[] logChars;
    }

    return rv;
}

ShaderDeserializationResult DeserializeFragmentShader(const FNode& file) {
    file.ReadContent();
    return DeserializeFragmentShader(file.DisposeContent());
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
        rv.deserializedShader = { f, Shader::Type::FRAGMENT, "", source, {} };
    }

    GLint bufferLength;
    glGetShaderiv(f, GL_INFO_LOG_LENGTH, &bufferLength);
    if (bufferLength > 1) {
        GLchar* logChars = new char[bufferLength + 1];
        glGetShaderInfoLog(f, bufferLength, NULL, logChars);
        std::string logString{ logChars };
        trim(logString);
        auto logs = SplitCompilerMessages(logString);
        for (const auto& log : logs) {
            rv.messages.emplace_back(ParseCompilerMessage(log));
        }
        delete[] logChars;
    }

    return rv;
}

ShaderDeserializationResult DeserializeComputeShader(const FNode& file) {
    file.ReadContent();
    return DeserializeComputeShader(file.DisposeContent());
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
        rv.deserializedShader = { c, Shader::Type::COMPUTE, "", source, {} };
    }

    GLint bufferLength;
    glGetShaderiv(c, GL_INFO_LOG_LENGTH, &bufferLength);
    if (bufferLength > 1) {
        GLchar* logChars = new char[bufferLength + 1];
        glGetShaderInfoLog(c, bufferLength, NULL, logChars);
        std::string logString{ logChars };
        trim(logString);
        auto logs = SplitCompilerMessages(logString);
        for (const auto& log : logs) {
            rv.messages.emplace_back(ParseCompilerMessage(log));
        }
        delete[] logChars;
    }

    return rv;
}
