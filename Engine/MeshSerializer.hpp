#pragma once

#include <string>
#include <functional>

#include <tinyxml2.h>

#include <Engine/Mesh.hpp>

/* This is the big bad bada-boom of serializers, this function will call appropriate serializers. See below for an overview! */
std::string SerializeMesh(const Mesh& mesh) noexcept;

/* Here is the general overview */
namespace MeshSerializer {

    /* ------- Type Definitons ------- */
    using HeaderCommentFunction = std::function<void(tinyxml2::XMLPrinter& printer, const Mesh& mesh)>;
    using SerializeFunction = std::function<void(tinyxml2::XMLPrinter& printer, const Mesh& mesh)>;
    using SerializeNameFunction = std::function<void(tinyxml2::XMLPrinter& printer, const std::string_view name)>;
    namespace Vertices {
        using SerializeFunction = std::function<void(tinyxml2::XMLPrinter& printer, const Mesh::VertexList& vertices)>;
        using SerializeVertexFunction = std::function<void(tinyxml2::XMLPrinter& printer, const Mesh::Vertex& vertex)>;
    }
    namespace Indices {
        using SerializeFunction = std::function<void(tinyxml2::XMLPrinter& printer, const Mesh::IndexList& indices)>;
    }


    /* --- Default Implementations --- */
    /* These are how NeoDoa will serialize by default. */
    void DefaultHeaderComment(tinyxml2::XMLPrinter& printer, [[maybe_unused]] const Mesh& mesh) noexcept;
    void DefaultSerialize(tinyxml2::XMLPrinter& printer, const Mesh& mesh) noexcept;
    void DefaultSerializeName(tinyxml2::XMLPrinter& printer, const std::string_view name) noexcept;
    namespace Vertices {
        void DefaultSerialize(tinyxml2::XMLPrinter& printer, const Mesh::VertexList& vertices);
        void DefaultSerializeVertex(tinyxml2::XMLPrinter& printer, const Mesh::Vertex& vertex);
    }
    namespace Indices {
        void DefaultSerialize(tinyxml2::XMLPrinter& printer, const Mesh::IndexList& indices);
    }


    /* ----- Serializer Functions ----- */
    inline HeaderCommentFunction HeaderComment{ DefaultHeaderComment }; /* Feel free to assign this your own function, if you need custom serialization */
    inline SerializeFunction Serialize{ DefaultSerialize };             /* Feel free to assign this your own function, if you need custom serialization */
    inline SerializeNameFunction SerializeName{ DefaultSerializeName }; /* Feel free to assign this your own function, if you need custom serialization */
    namespace Vertices {
        inline SerializeFunction Serialize{ DefaultSerialize };
        inline SerializeVertexFunction SerializeVertex{ DefaultSerializeVertex };
    }
    namespace Indices {
        inline SerializeFunction Serialize{ DefaultSerialize };
    }
}

/* Some Helper functions to trivially serialize (not-so) trivial data types */
namespace MeshSerializer::Helpers {

}