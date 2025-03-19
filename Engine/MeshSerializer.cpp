#include <Engine/MeshSerializer.hpp>

#include <utility>

#include <Utility/NameOf.hpp>

std::string SerializeMesh(const Mesh& mesh) noexcept {
    tinyxml2::XMLPrinter printer;
    MeshSerializer::HeaderComment(printer, mesh);
    MeshSerializer::Serialize(printer, mesh);

    return printer.CStr();
}

void MeshSerializer::DefaultHeaderComment(tinyxml2::XMLPrinter& printer, [[maybe_unused]] const Mesh& mesh) noexcept {
    printer.PushComment("WARNING!! This file is not for editing! Don't!");
    printer.PushComment("OpenMeshFileFormat_v19_01_2025");
}
void MeshSerializer::DefaultSerialize(tinyxml2::XMLPrinter& printer, const Mesh& mesh) noexcept {
    printer.OpenElement("mesh");
    {
        SerializeName(printer, mesh.Name);
        Vertices::Serialize(printer, mesh.Vertices.value_or(Mesh::VertexList{}));
        Indices::Serialize(printer, mesh.Indices.value_or(Mesh::IndexList{}));
    }
    printer.CloseElement();
}
void MeshSerializer::DefaultSerializeName(tinyxml2::XMLPrinter& printer, const std::string_view name) noexcept {
    printer.PushAttribute("name", name.data());
}

void MeshSerializer::Vertices::DefaultSerialize(tinyxml2::XMLPrinter& printer, const Mesh::VertexList& vertices) {
    printer.OpenElement("vertices");
    {
        for (const Mesh::Vertex& vertex : vertices) {
            SerializeVertex(printer, vertex);
        }
    }
    printer.CloseElement();
}
void MeshSerializer::Vertices::DefaultSerializeVertex(tinyxml2::XMLPrinter& printer, const Mesh::Vertex& vertex) {
    printer.OpenElement("vertex");
    {
        printer.OpenElement("position");
        printer.PushAttribute("x", vertex.Position.x);
        printer.PushAttribute("y", vertex.Position.y);
        printer.PushAttribute("z", vertex.Position.z);
        printer.CloseElement();

        printer.OpenElement("normal");
        printer.PushAttribute("x", vertex.Normal.x);
        printer.PushAttribute("y", vertex.Normal.y);
        printer.PushAttribute("z", vertex.Normal.z);
        printer.CloseElement();

        printer.OpenElement("texCoords");
        printer.PushAttribute("u", vertex.TexCoords.x);
        printer.PushAttribute("v", vertex.TexCoords.y);
        printer.CloseElement();
    }
    printer.CloseElement();
}

void MeshSerializer::Indices::DefaultSerialize(tinyxml2::XMLPrinter& printer, const Mesh::IndexList& indices) {
    printer.OpenElement("indices");
    {
        for (const auto index : indices) {
            SerializeIndex(printer, index);
        }
    }
    printer.CloseElement();
}
void MeshSerializer::Indices::DefaultSerializeIndex(tinyxml2::XMLPrinter& printer, const Mesh::IndexList::value_type& index) {
    printer.OpenElement("index");
    printer.PushText(index);
    printer.CloseElement();
}
