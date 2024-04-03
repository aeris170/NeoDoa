#include <Engine/ShaderProgramSerializer.hpp>

#include <Engine/UUID.hpp>

std::string SerializeShaderProgram(const ShaderProgram& program) {
    tinyxml2::XMLPrinter printer;
    ShaderProgramSerializer::HeaderComment(printer, program);
    ShaderProgramSerializer::Serialize(printer, program);

    return printer.CStr();
}

void ShaderProgramSerializer::DefaultHeaderComment(tinyxml2::XMLPrinter& printer, [[maybe_unused]] const ShaderProgram& program) {
    printer.PushComment("WARNING!! This file is not for editing! Don't!");
}
void ShaderProgramSerializer::DefaultSerialize(tinyxml2::XMLPrinter& printer, const ShaderProgram& program) {
    printer.OpenElement("program");
    {
        ProgramConfig::Serialize(printer, program);
        Shaders::Serialize(printer, program);
    }
    printer.CloseElement();
}

void ShaderProgramSerializer::ProgramConfig::DefaultSerialize(tinyxml2::XMLPrinter& printer, const ShaderProgram& program) {
    printer.OpenElement("config");
    {
        SerializeName(printer, program.Name);
    }
    printer.CloseElement(); // config close
}
void ShaderProgramSerializer::ProgramConfig::DefaultSerializeName(tinyxml2::XMLPrinter& printer, ::std::string_view name) {
    printer.PushAttribute("name", name.data());
}

void ShaderProgramSerializer::Shaders::DefaultSerialize(tinyxml2::XMLPrinter& printer, const ShaderProgram& program) {
    printer.OpenElement("shaders");
    {
        SerializeVertexShader(printer, program.VertexShader);
        SerializeTessellationControlShader(printer, program.TessellationControlShader);
        SerializeTessellationEvaluationShader(printer, program.TessellationEvaluationShader);
        SerializeGeometryShader(printer, program.GeometryShader);
        SerializeFragmentShader(printer, program.FragmentShader);
        SerializeComputeShader(printer, program.ComputeShader);
    }
    printer.CloseElement(); // shaders close
}
void ShaderProgramSerializer::Shaders::DefaultSerializeVertexShader(tinyxml2::XMLPrinter& printer, const UUID& vertexShader) {
    printer.OpenElement("vertex-shader");
    printer.PushAttribute("uuid", vertexShader);
    printer.CloseElement();
}
void ShaderProgramSerializer::Shaders::DefaultSerializeTessellationControlShader(tinyxml2::XMLPrinter& printer, const UUID& tessellationControlShader) {
    printer.OpenElement("tess-ctrl-shader");
    printer.PushAttribute("uuid", tessellationControlShader);
    printer.CloseElement();
}
void ShaderProgramSerializer::Shaders::DefaultSerializeTessellationEvaluationShader(tinyxml2::XMLPrinter& printer, const UUID& tessellationEvaluationShader) {
    printer.OpenElement("tess-eval-shader");
    printer.PushAttribute("uuid", tessellationEvaluationShader);
    printer.CloseElement();
}
void ShaderProgramSerializer::Shaders::DefaultSerializeGeometryShader(tinyxml2::XMLPrinter& printer, const UUID& geometryShader) {
    printer.OpenElement("geometry-shader");
    printer.PushAttribute("uuid", geometryShader);
    printer.CloseElement();
}
void ShaderProgramSerializer::Shaders::DefaultSerializeFragmentShader(tinyxml2::XMLPrinter& printer, const UUID& fragmentShader) {
    printer.OpenElement("fragment-shader");
    printer.PushAttribute("uuid", fragmentShader);
    printer.CloseElement();
}
void ShaderProgramSerializer::Shaders::DefaultSerializeComputeShader(tinyxml2::XMLPrinter& printer, const UUID& computeShader) {
    printer.OpenElement("compute-shader");
    printer.PushAttribute("uuid", computeShader);
    printer.CloseElement();
}
