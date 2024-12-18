#include <Engine/MaterialSerializer.hpp>

#include <cassert>

#include <tinyxml2.h>

#include <Utility/NameOf.hpp>
#include <Utility/TemplateUtilities.hpp>

#include <Engine/Shader.hpp>
#include <Engine/Material.hpp>

std::string SerializeMaterial(const Material& material) noexcept {
    tinyxml2::XMLPrinter printer;
    MaterialSerializer::HeaderComment(printer, material);
    MaterialSerializer::Serialize(printer, material);

    return printer.CStr();
}

void MaterialSerializer::DefaultHeaderComment(tinyxml2::XMLPrinter& printer, [[maybe_unused]] const Material& material) noexcept {
    printer.PushComment("WARNING!! This file is not for editing! Don't!");
}
void MaterialSerializer::DefaultSerialize(tinyxml2::XMLPrinter& printer, const Material& material) noexcept {
    printer.OpenElement("material");
    {
        Program::Serialize(printer, material);
        Uniforms::Serialize(printer, material);
    }
    printer.CloseElement();
}

void MaterialSerializer::Program::DefaultSerialize(tinyxml2::XMLPrinter& printer, const Material& material) noexcept {
    printer.OpenElement("program");
    {
        SerializeID(printer, material.ShaderProgram);
    }
    printer.CloseElement();
}
void MaterialSerializer::Program::DefaultSerializeID(tinyxml2::XMLPrinter& printer, const UUID id) noexcept {
    printer.PushAttribute("id", id);
}

void MaterialSerializer::Uniforms::DefaultSerialize(tinyxml2::XMLPrinter& printer, const Material& material) noexcept {
    printer.OpenElement("uniforms");
    {
        SerializeVertexUniforms(printer, material.VertexUniforms);
        SerializeTessellationControlUniforms(printer, material.TessellationControlUniforms);
        SerializeTessellationEvaluationUniforms(printer, material.TessellationEvaluationUniforms);
        SerializeGeometryUniforms(printer, material.GeometryUniforms);
        SerializeFragmentUniforms(printer, material.FragmentUniforms);
    }
    printer.CloseElement();
}
void MaterialSerializer::Uniforms::DefaultSerializeVertexUniforms(tinyxml2::XMLPrinter& printer, const Material::Uniforms& uniforms) noexcept {
    printer.OpenElement("vertex-uniforms");
    {
        for (const auto& uniform : uniforms.GetAll()) {
            SerializeUniform(printer, uniform);
        }
    }
    printer.CloseElement();
}
void MaterialSerializer::Uniforms::DefaultSerializeTessellationControlUniforms(tinyxml2::XMLPrinter& printer, const Material::Uniforms& uniforms) noexcept {
    printer.OpenElement("tessellation-control-uniforms");
    {
        for (const auto& uniform : uniforms.GetAll()) {
            SerializeUniform(printer, uniform);
        }
    }
    printer.CloseElement();
}
void MaterialSerializer::Uniforms::DefaultSerializeTessellationEvaluationUniforms(tinyxml2::XMLPrinter& printer, const Material::Uniforms& uniforms) noexcept {
    printer.OpenElement("tessellation-evaluation-uniforms");
    {
        for (const auto& uniform : uniforms.GetAll()) {
            SerializeUniform(printer, uniform);
        }
    }
    printer.CloseElement();
}
void MaterialSerializer::Uniforms::DefaultSerializeGeometryUniforms(tinyxml2::XMLPrinter& printer, const Material::Uniforms& uniforms) noexcept {
    printer.OpenElement("geometry-uniforms");
    {
        for (const auto& uniform : uniforms.GetAll()) {
            SerializeUniform(printer, uniform);
        }
    }
    printer.CloseElement();
}
void MaterialSerializer::Uniforms::DefaultSerializeFragmentUniforms(tinyxml2::XMLPrinter& printer, const Material::Uniforms& uniforms) noexcept {
    printer.OpenElement("fragment-uniforms");
    {
        for (const auto& uniform : uniforms.GetAll()) {
            SerializeUniform(printer, uniform);
        }
    }
    printer.CloseElement();
}
void MaterialSerializer::Uniforms::DefaultSerializeUniform(tinyxml2::XMLPrinter& printer, const UniformValue& uniform) noexcept {
    printer.OpenElement("uniform");
    {
        printer.PushAttribute("location", uniform.Location);
        printer.PushAttribute("name", uniform.Name.c_str());
        printer.PushAttribute("type", Helpers::ExtractUniformTypeString(uniform).data());

        std::visit(overloaded::lambda{
            [&printer](const Uniform1f& u) {
                Helpers::SerializeUniform1f(printer, u);
            },
            [&printer](const Uniform2f& u) {
                Helpers::SerializeUniform2f(printer, u);
            },
            [&printer](const Uniform3f& u) {
                Helpers::SerializeUniform3f(printer, u);
            },
            [&printer](const Uniform4f& u) {
                Helpers::SerializeUniform4f(printer, u);
            },

            [&printer](const Uniform1i& u) {
                Helpers::SerializeUniform1i(printer, u);
            },
            [&printer](const Uniform2i& u) {
                Helpers::SerializeUniform2i(printer, u);
            },
            [&printer](const Uniform3i& u) {
                Helpers::SerializeUniform3i(printer, u);
            },
            [&printer](const Uniform4i& u) {
                Helpers::SerializeUniform4i(printer, u);
            },

            [&printer](const Uniform1ui& u) {
                Helpers::SerializeUniform1ui(printer, u);
            },
            [&printer](const Uniform2ui& u) {
                Helpers::SerializeUniform2ui(printer, u);
            },
            [&printer](const Uniform3ui& u) {
                Helpers::SerializeUniform3ui(printer, u);
            },
            [&printer](const Uniform4ui& u) {
                Helpers::SerializeUniform4ui(printer, u);
            },

            [&printer](const UniformMatrix2f& u) {
                Helpers::SerializeUniformMatrix2f(printer, u);
            },
            [&printer](const UniformMatrix3f& u) {
                Helpers::SerializeUniformMatrix3f(printer, u);
            },
            [&printer](const UniformMatrix4f& u) {
                Helpers::SerializeUniformMatrix4f(printer, u);
            },
            [&printer](const UniformMatrix2x3f& u) {
                Helpers::SerializeUniformMatrix2x3f(printer, u);
            },
            [&printer](const UniformMatrix3x2f& u) {
                Helpers::SerializeUniformMatrix3x2f(printer, u);
            },
            [&printer](const UniformMatrix2x4f& u) {
                Helpers::SerializeUniformMatrix2x4f(printer, u);
            },
            [&printer](const UniformMatrix4x2f& u) {
                Helpers::SerializeUniformMatrix4x2f(printer, u);
            },
            [&printer](const UniformMatrix3x4f& u) {
                Helpers::SerializeUniformMatrix3x4f(printer, u);
            },
            [&printer](const UniformMatrix4x3f& u) {
                Helpers::SerializeUniformMatrix4x3f(printer, u);
            },

            [&printer](const UniformSampler2D& u) {
                Helpers::SerializeUniformSampler2D(printer, u);
            }
        }, uniform.Value);
    }
    printer.CloseElement();
}

std::string MaterialSerializer::Helpers::ExtractUniformTypeString(const UniformValue& uniform) noexcept {
    std::string rv;

    std::visit(overloaded::lambda{
        [&rv]([[maybe_unused]] const Uniform1f& _) { rv = nameof(Uniform1f); },
        [&rv]([[maybe_unused]] const Uniform2f& _) { rv = nameof(Uniform2f); },
        [&rv]([[maybe_unused]] const Uniform3f& _) { rv = nameof(Uniform3f); },
        [&rv]([[maybe_unused]] const Uniform4f& _) { rv = nameof(Uniform4f); },

        [&rv]([[maybe_unused]] const Uniform1i& _) { rv = nameof(Uniform1i); },
        [&rv]([[maybe_unused]] const Uniform2i& _) { rv = nameof(Uniform2i); },
        [&rv]([[maybe_unused]] const Uniform3i& _) { rv = nameof(Uniform3i); },
        [&rv]([[maybe_unused]] const Uniform4i& _) { rv = nameof(Uniform4i); },

        [&rv]([[maybe_unused]] const Uniform1ui& _) { rv = nameof(Uniform1ui); },
        [&rv]([[maybe_unused]] const Uniform2ui& _) { rv = nameof(Uniform2ui); },
        [&rv]([[maybe_unused]] const Uniform3ui& _) { rv = nameof(Uniform3ui); },
        [&rv]([[maybe_unused]] const Uniform4ui& _) { rv = nameof(Uniform4ui); },

        [&rv]([[maybe_unused]] const UniformMatrix2f& _) { rv = nameof(UniformMatrix2f); },
        [&rv]([[maybe_unused]] const UniformMatrix3f& _) { rv = nameof(UniformMatrix3f); },
        [&rv]([[maybe_unused]] const UniformMatrix4f& _) { rv = nameof(UniformMatrix4f); },
        [&rv]([[maybe_unused]] const UniformMatrix2x3f& _) { rv = nameof(UniformMatrix2x3f); },
        [&rv]([[maybe_unused]] const UniformMatrix3x2f& _) { rv = nameof(UniformMatrix3x2f); },
        [&rv]([[maybe_unused]] const UniformMatrix2x4f& _) { rv = nameof(UniformMatrix2x4f); },
        [&rv]([[maybe_unused]] const UniformMatrix4x2f& _) { rv = nameof(UniformMatrix4x2f); },
        [&rv]([[maybe_unused]] const UniformMatrix3x4f& _) { rv = nameof(UniformMatrix3x4f); },
        [&rv]([[maybe_unused]] const UniformMatrix4x3f& _) { rv = nameof(UniformMatrix4x3f); },

        [&rv]([[maybe_unused]] const UniformSampler2D& _) { rv = nameof(UniformSampler2D); }
    }, uniform.Value);

    return rv;
}

void MaterialSerializer::Helpers::SerializeUniform1f(tinyxml2::XMLPrinter& printer, const Uniform1f& uniformValue) noexcept {
    printer.OpenElement("value");
    {
        printer.PushAttribute("index0", uniformValue[0]);
    }
    printer.CloseElement();
}
void MaterialSerializer::Helpers::SerializeUniform2f(tinyxml2::XMLPrinter& printer, const Uniform2f& uniformValue) noexcept {
    printer.OpenElement("value");
    {
        printer.PushAttribute("index0", uniformValue[0]);
        printer.PushAttribute("index1", uniformValue[1]);
    }
    printer.CloseElement();
}
void MaterialSerializer::Helpers::SerializeUniform3f(tinyxml2::XMLPrinter& printer, const Uniform3f& uniformValue) noexcept {
    printer.OpenElement("value");
    {
        printer.PushAttribute("index0", uniformValue[0]);
        printer.PushAttribute("index1", uniformValue[1]);
        printer.PushAttribute("index2", uniformValue[2]);
    }
    printer.CloseElement();
}
void MaterialSerializer::Helpers::SerializeUniform4f(tinyxml2::XMLPrinter& printer, const Uniform4f& uniformValue) noexcept {
    printer.OpenElement("value");
    {
        printer.PushAttribute("index0", uniformValue[0]);
        printer.PushAttribute("index1", uniformValue[1]);
        printer.PushAttribute("index2", uniformValue[2]);
        printer.PushAttribute("index3", uniformValue[3]);
    }
    printer.CloseElement();
}

void MaterialSerializer::Helpers::SerializeUniform1i(tinyxml2::XMLPrinter& printer, const Uniform1i& uniformValue) noexcept {
    printer.OpenElement("value");
    {
        printer.PushAttribute("index0", uniformValue[0]);
    }
    printer.CloseElement();
}
void MaterialSerializer::Helpers::SerializeUniform2i(tinyxml2::XMLPrinter& printer, const Uniform2i& uniformValue) noexcept {
    printer.OpenElement("value");
    {
        printer.PushAttribute("index0", uniformValue[0]);
        printer.PushAttribute("index1", uniformValue[1]);
    }
    printer.CloseElement();
}
void MaterialSerializer::Helpers::SerializeUniform3i(tinyxml2::XMLPrinter& printer, const Uniform3i& uniformValue) noexcept {
    printer.OpenElement("value");
    {
        printer.PushAttribute("index0", uniformValue[0]);
        printer.PushAttribute("index1", uniformValue[1]);
        printer.PushAttribute("index2", uniformValue[2]);
    }
    printer.CloseElement();
}
void MaterialSerializer::Helpers::SerializeUniform4i(tinyxml2::XMLPrinter& printer, const Uniform4i& uniformValue) noexcept {
    printer.OpenElement("value");
    {
        printer.PushAttribute("index0", uniformValue[0]);
        printer.PushAttribute("index1", uniformValue[1]);
        printer.PushAttribute("index2", uniformValue[2]);
        printer.PushAttribute("index3", uniformValue[3]);
    }
    printer.CloseElement();
}

void MaterialSerializer::Helpers::SerializeUniform1ui(tinyxml2::XMLPrinter& printer, const Uniform1ui& uniformValue) noexcept {
    printer.OpenElement("value");
    {
        printer.PushAttribute("index0", uniformValue[0]);
    }
    printer.CloseElement();
}
void MaterialSerializer::Helpers::SerializeUniform2ui(tinyxml2::XMLPrinter& printer, const Uniform2ui& uniformValue) noexcept {
    printer.OpenElement("value");
    {
        printer.PushAttribute("index0", uniformValue[0]);
        printer.PushAttribute("index1", uniformValue[1]);
    }
    printer.CloseElement();
}
void MaterialSerializer::Helpers::SerializeUniform3ui(tinyxml2::XMLPrinter& printer, const Uniform3ui& uniformValue) noexcept {
    printer.OpenElement("value");
    {
        printer.PushAttribute("index0", uniformValue[0]);
        printer.PushAttribute("index1", uniformValue[1]);
        printer.PushAttribute("index2", uniformValue[2]);
    }
    printer.CloseElement();
}
void MaterialSerializer::Helpers::SerializeUniform4ui(tinyxml2::XMLPrinter& printer, const Uniform4ui& uniformValue) noexcept {
    printer.OpenElement("value");
    {
        printer.PushAttribute("index0", uniformValue[0]);
        printer.PushAttribute("index1", uniformValue[1]);
        printer.PushAttribute("index2", uniformValue[2]);
        printer.PushAttribute("index3", uniformValue[3]);
    }
    printer.CloseElement();
}

void MaterialSerializer::Helpers::SerializeUniformMatrix2f(tinyxml2::XMLPrinter& printer, const UniformMatrix2f& uniformValue) noexcept {
    printer.OpenElement("value");
    {
        printer.PushAttribute("index00", uniformValue[0][0]);
        printer.PushAttribute("index01", uniformValue[0][1]);

        printer.PushAttribute("index10", uniformValue[1][0]);
        printer.PushAttribute("index11", uniformValue[1][1]);
    }
    printer.CloseElement();
}
void MaterialSerializer::Helpers::SerializeUniformMatrix3f(tinyxml2::XMLPrinter& printer, const UniformMatrix3f& uniformValue) noexcept {
    printer.OpenElement("value");
    {
        printer.PushAttribute("index00", uniformValue[0][0]);
        printer.PushAttribute("index01", uniformValue[0][1]);
        printer.PushAttribute("index02", uniformValue[0][2]);

        printer.PushAttribute("index10", uniformValue[1][0]);
        printer.PushAttribute("index11", uniformValue[1][1]);
        printer.PushAttribute("index12", uniformValue[1][2]);

        printer.PushAttribute("index20", uniformValue[2][0]);
        printer.PushAttribute("index21", uniformValue[2][1]);
        printer.PushAttribute("index22", uniformValue[2][2]);
    }
    printer.CloseElement();
}
void MaterialSerializer::Helpers::SerializeUniformMatrix4f(tinyxml2::XMLPrinter& printer, const UniformMatrix4f& uniformValue) noexcept {
    printer.OpenElement("value");
    {
        printer.PushAttribute("index00", uniformValue[0][0]);
        printer.PushAttribute("index01", uniformValue[0][1]);
        printer.PushAttribute("index02", uniformValue[0][2]);
        printer.PushAttribute("index03", uniformValue[0][3]);

        printer.PushAttribute("index10", uniformValue[1][0]);
        printer.PushAttribute("index11", uniformValue[1][1]);
        printer.PushAttribute("index12", uniformValue[1][2]);
        printer.PushAttribute("index13", uniformValue[2][3]);

        printer.PushAttribute("index20", uniformValue[2][0]);
        printer.PushAttribute("index21", uniformValue[2][1]);
        printer.PushAttribute("index22", uniformValue[2][2]);
        printer.PushAttribute("index23", uniformValue[2][3]);

        printer.PushAttribute("index30", uniformValue[3][0]);
        printer.PushAttribute("index31", uniformValue[3][1]);
        printer.PushAttribute("index32", uniformValue[3][2]);
        printer.PushAttribute("index33", uniformValue[3][3]);
    }
    printer.CloseElement();
}
void MaterialSerializer::Helpers::SerializeUniformMatrix2x3f(tinyxml2::XMLPrinter& printer, const UniformMatrix2x3f& uniformValue) noexcept {
    printer.OpenElement("value");
    {
        printer.PushAttribute("index00", uniformValue[0][0]);
        printer.PushAttribute("index01", uniformValue[0][1]);

        printer.PushAttribute("index10", uniformValue[1][0]);
        printer.PushAttribute("index11", uniformValue[1][1]);

        printer.PushAttribute("index20", uniformValue[2][0]);
        printer.PushAttribute("index21", uniformValue[2][1]);
    }
    printer.CloseElement();
}
void MaterialSerializer::Helpers::SerializeUniformMatrix3x2f(tinyxml2::XMLPrinter& printer, const UniformMatrix3x2f& uniformValue) noexcept {
    printer.OpenElement("value");
    {
        printer.PushAttribute("index00", uniformValue[0][0]);
        printer.PushAttribute("index01", uniformValue[0][1]);
        printer.PushAttribute("index02", uniformValue[0][2]);

        printer.PushAttribute("index10", uniformValue[1][0]);
        printer.PushAttribute("index11", uniformValue[1][1]);
        printer.PushAttribute("index12", uniformValue[1][2]);
    }
    printer.CloseElement();
}
void MaterialSerializer::Helpers::SerializeUniformMatrix2x4f(tinyxml2::XMLPrinter& printer, const UniformMatrix2x4f& uniformValue) noexcept {
    printer.OpenElement("value");
    {
        printer.PushAttribute("index00", uniformValue[0][0]);
        printer.PushAttribute("index01", uniformValue[0][1]);

        printer.PushAttribute("index10", uniformValue[1][0]);
        printer.PushAttribute("index11", uniformValue[1][1]);

        printer.PushAttribute("index20", uniformValue[2][0]);
        printer.PushAttribute("index21", uniformValue[2][1]);

        printer.PushAttribute("index30", uniformValue[3][0]);
        printer.PushAttribute("index31", uniformValue[3][1]);
    }
    printer.CloseElement();
}
void MaterialSerializer::Helpers::SerializeUniformMatrix4x2f(tinyxml2::XMLPrinter& printer, const UniformMatrix4x2f& uniformValue) noexcept {
    printer.OpenElement("value");
    {
        printer.PushAttribute("index00", uniformValue[0][0]);
        printer.PushAttribute("index01", uniformValue[0][1]);
        printer.PushAttribute("index02", uniformValue[0][2]);
        printer.PushAttribute("index03", uniformValue[0][3]);

        printer.PushAttribute("index10", uniformValue[1][0]);
        printer.PushAttribute("index11", uniformValue[1][1]);
        printer.PushAttribute("index12", uniformValue[1][2]);
        printer.PushAttribute("index13", uniformValue[1][3]);
    }
    printer.CloseElement();
}
void MaterialSerializer::Helpers::SerializeUniformMatrix3x4f(tinyxml2::XMLPrinter& printer, const UniformMatrix3x4f& uniformValue) noexcept {
    printer.OpenElement("value");
    {
        printer.PushAttribute("index00", uniformValue[0][0]);
        printer.PushAttribute("index01", uniformValue[0][1]);
        printer.PushAttribute("index02", uniformValue[0][2]);

        printer.PushAttribute("index10", uniformValue[1][0]);
        printer.PushAttribute("index11", uniformValue[1][1]);
        printer.PushAttribute("index12", uniformValue[1][2]);

        printer.PushAttribute("index20", uniformValue[2][0]);
        printer.PushAttribute("index21", uniformValue[2][1]);
        printer.PushAttribute("index22", uniformValue[2][2]);

        printer.PushAttribute("index30", uniformValue[3][0]);
        printer.PushAttribute("index31", uniformValue[3][1]);
        printer.PushAttribute("index32", uniformValue[3][2]);
    }
    printer.CloseElement();
}
void MaterialSerializer::Helpers::SerializeUniformMatrix4x3f(tinyxml2::XMLPrinter& printer, const UniformMatrix4x3f& uniformValue) noexcept {
    printer.OpenElement("value");
    {
        printer.PushAttribute("index00", uniformValue[0][0]);
        printer.PushAttribute("index01", uniformValue[0][1]);
        printer.PushAttribute("index02", uniformValue[0][2]);
        printer.PushAttribute("index03", uniformValue[0][3]);

        printer.PushAttribute("index10", uniformValue[1][0]);
        printer.PushAttribute("index11", uniformValue[1][1]);
        printer.PushAttribute("index12", uniformValue[1][2]);
        printer.PushAttribute("index13", uniformValue[1][3]);

        printer.PushAttribute("index20", uniformValue[2][0]);
        printer.PushAttribute("index21", uniformValue[2][1]);
        printer.PushAttribute("index22", uniformValue[2][2]);
        printer.PushAttribute("index23", uniformValue[2][3]);
    }
    printer.CloseElement();
}

void MaterialSerializer::Helpers::SerializeUniformSampler2D(tinyxml2::XMLPrinter& printer, const UniformSampler2D& uniformValue) noexcept {
    printer.OpenElement("value");
    {
        printer.PushAttribute("texture", uniformValue.textureUUID);
        printer.PushAttribute("sampler", uniformValue.samplerUUID);
    }
    printer.CloseElement();
}
